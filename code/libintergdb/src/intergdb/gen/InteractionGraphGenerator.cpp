#include <intergdb/gen/InteractionGraphGenerator.h>

#include <intergdb/core/Helper.h>

#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/graph/rmat_graph_generator.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include <iterator>
#include <algorithm>

using namespace std;
using namespace intergdb::core;
using namespace intergdb::gen;

double InteractionGraphGenerator::Conf::
    convertPerDayCountToInterArrivalDelayInMillisecs(size_t perDay)
{
    return 1000.0 / (static_cast<double>(perDay) / (60.0*60.0*24.0));
}

double InteractionGraphGenerator::Conf::
    getErdosReyniEdgeProb(size_t numVertices, size_t numEdges)
{
    double m = numEdges;
    double n = numVertices;
    return  (2*m) / (n*(n-1));
}

string InteractionGraphGenerator::getGraphTypeName(GraphType type)
{
    switch(type)
    {
        case ScaleFree: return std::string("ScaleFree");
        case ErdosReyni: return std::string("ErdosReyni");
    }
    assert(!"cannot happen");
    return "";
}

void InteractionGraphGenerator::buildGraph()
{
    Graph * g = nullptr;
    { // create the graph
        size_t n = conf_.numVertices();
        size_t m = conf_.numEdges();
        boost::minstd_rand gen;
        gen.seed(boost::minstd_rand::default_seed);
        if (conf_.graphType()==ErdosReyni) {
            typedef boost::sorted_erdos_renyi_iterator<boost::minstd_rand, Graph> ERGen;
            double p = Conf::getErdosReyniEdgeProb(n, m);
            g = new Graph(ERGen(gen, n, p), ERGen(), n);
        } else {
            struct no_self_loops {
                no_self_loops() {}
                bool operator()(const std::pair<int,int>& e)
                    { return e.first != e.second; }
            };
            typedef boost::unique_rmat_iterator<boost::minstd_rand, Graph> RMATGen;
            g = new Graph(boost::make_filter_iterator<no_self_loops>(RMATGen(gen, n, m, 0.57, 0.19, 0.19, 0.05), RMATGen()),
                          boost::make_filter_iterator<no_self_loops>(RMATGen(), RMATGen()), n);
        }
        graph_.reset(g);
    }
    { // setup random variables
        double expMean = 1.0/conf_.interArrivalTimeMean();
        rgenForExp_.seed(rgenForExp_.default_seed);
        expDist_.reset(new ExpVariate(rgenForExp_, boost::exponential_distribution<>(expMean)));
        rgenForUnif_.seed(rgenForUnif_.default_seed);
        unifDist_.reset(new UnifVariate(rgenForUnif_, boost::uniform_real<>(0.0, 1.0)));
        popularitySkew_.reset(new Zipf(conf_.popularityZipfParam(), conf_.popularityGroupCount()));
    }
    { // setup vertex indices
        vertexIndex_ = get(boost::vertex_index, *graph_);
        vertexInfo_.clear();
        verticesByPopularity_.clear();
        verticesByPopularity_.resize(conf_.popularityGroupCount(), vector<VertexId>());
        for (size_t vertex=0; vertex<conf_.numVertices(); ++vertex) {
            size_t popularityIndex = std::floor((*unifDist_)()*(conf_.popularityGroupCount()));
            //1+std::floor((*unifDist_)()*(conf_.popularityGroupCount()-1));
            verticesByPopularity_[popularityIndex].push_back(vertex);
            VertexInfo vertexInfo;
            vertexInfo.time = 0.0;
            vertexInfo.popularityIndex = popularityIndex;
            vertexInfo.currentPopularityIndex = popularityIndex;
            vertexInfo_[vertex] = vertexInfo;
        }
    }
    time_ = 0;
}

InteractionGraphGenerator::VertexIterator::VertexIterator(Graph & g, GraphIndexMap * map)
  : map_(map), iter_(vertices(g))
{}

bool InteractionGraphGenerator::VertexIterator::isValid()
{
    return iter_.first!=iter_.second;
}

void InteractionGraphGenerator::VertexIterator::next()
{
    ++iter_.first;
}

VertexId InteractionGraphGenerator::VertexIterator::getVertex()
{
     return (*map_)[*iter_.first];
}

InteractionGraphGenerator::VertexIterator InteractionGraphGenerator::getVertexIterator()
{
    return VertexIterator(*graph_, &vertexIndex_);
}


VertexId InteractionGraphGenerator::getFromVertex()
{
    VertexId from;
    bool found = false;
    while (!found) {
        size_t popularityIndex = 0;
        vector<VertexId> const * vertices = NULL;
        while (vertices == NULL || vertices->size()==0) {
            popularityIndex = popularitySkew_->getRandomValue();
            vertices = & verticesByPopularity_[popularityIndex];
        }
        size_t index = std::floor((*unifDist_)() * (*vertices).size());
        from = (*vertices)[index];
        VertexDesc vertexDesc = boost::vertex(from, *graph_);
        size_t numEdges = boost::out_degree(vertexDesc, *graph_);
        found = (numEdges>0);
    }
    return from;
}

VertexId InteractionGraphGenerator::getToVertex(VertexId from)
{
    EdgeIter ei, ei_end;
    VertexDesc fromDesc = boost::vertex(from, *graph_);
    size_t numEdges = boost::out_degree(fromDesc, *graph_);
    boost::tie(ei, ei_end) = boost::out_edges(fromDesc, *graph_);
    size_t index = std::floor((*unifDist_)()*numEdges);
    std::advance(ei, index);
    VertexDesc toDesc = boost::target(*ei, *graph_);
    VertexId to = vertexIndex_[toDesc];
    return to;
}

void InteractionGraphGenerator::readjustPriorities(VertexId from, VertexId to)
{
    assert(from!=to);
    {
        auto & vertexInfo = vertexInfo_[from];
        if (vertexInfo.currentPopularityIndex != vertexInfo.popularityIndex) {
            auto & current = verticesByPopularity_[vertexInfo.currentPopularityIndex];
            auto & original = verticesByPopularity_[vertexInfo.popularityIndex];
            auto it = find(current.begin(), current.end(), from);
            assert(it!=current.end());
            current.erase(it);
            original.push_back(from);
            vertexInfo.currentPopularityIndex = vertexInfo.popularityIndex;
        }
    }
    {
        auto & vertexInfo = vertexInfo_[to];
        if (vertexInfo.currentPopularityIndex != 0) {
            size_t destinationIndex = std::floor((*unifDist_)()*vertexInfo.currentPopularityIndex);
            auto & current = verticesByPopularity_[vertexInfo.currentPopularityIndex];
            auto & destination = verticesByPopularity_[destinationIndex];
            auto it = find(current.begin(), current.end(), to);
            assert(it!=current.end());
            current.erase(it);
            destination.push_back(to);
            vertexInfo.currentPopularityIndex = destinationIndex;
        }
    }
    //auto & vertices = verticesByPopularity_[0];
    //cerr << vertices.size() << endl;
    //for(size_t i=0, iu=vertices.size(); i<iu; ++i)
    //    cerr << vertices[i] << " ";
    //cerr << endl;
}

Edge InteractionGraphGenerator::generateNextEdge()
{
    { // update time
        double interArrival = (*expDist_)();
        double oldTime = time_;
        time_ += interArrival;
        if(!(time_>oldTime))
            throw runtime_error("time should be monotonically increasing");
    }

    /* TODO: remove begin
    while (true) {
        size_t from = std::floor((*unifDist_)() * 20);
        size_t to = std::floor((*unifDist_)() * 20);
        if (from==to)
            continue;
        return core::Edge(from, to, time);
    }
    //TODO: remove end */

    VertexId from = getFromVertex();
    VertexId to = getToVertex(from);
    readjustPriorities(from, to);

    // not needed
    vertexInfo_[from].time = time_;
    vertexInfo_[to].time = time_;

    return core::Edge(from, to, time_);
}

// not needed
VertexId InteractionGraphGenerator::getRandomPopularVertex()
{
    auto const & vertices = verticesByPopularity_[0];
    size_t index = std::floor((*unifDist_)() * vertices.size());
    VertexId from = vertices[index];
    return from;
}



