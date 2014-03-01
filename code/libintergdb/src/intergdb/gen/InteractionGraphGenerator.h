#ifndef INTERGEN_INTERACTIONGRAPHGENERATOR_H
#define INTERGEN_INTERACTIONGRAPHGENERATOR_H

#include <intergdb/core/Types.h>
#include <intergdb/core/Edge.h>
#include <intergdb/gen/Zipf.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/exponential_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random.hpp>

#include <tr1/unordered_map>

#include <cstdlib>
#include <inttypes.h>

namespace intergdb { namespace gen
{
    class InteractionGraphGenerator
    {
    private:
        typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;
        typedef boost::property_map<Graph, boost::vertex_index_t>::type GraphIndexMap;
        typedef boost::graph_traits<Graph>::vertex_iterator VertexIter;
        typedef boost::graph_traits<Graph>::out_edge_iterator EdgeIter;
        typedef boost::graph_traits<Graph>::vertex_descriptor VertexDesc;
        typedef boost::graph_traits<Graph>::edge_descriptor EdgeDesc;
    public:
        class VertexIterator
        {
        private:
            typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
        public:
            VertexIterator(Graph & g, GraphIndexMap * map);
            bool isValid();
            void next();
            intergdb::core::VertexId getVertex();
        private:
            GraphIndexMap * map_;
            std::pair<vertex_iter,vertex_iter> iter_;
        };
        enum GraphType
        {
            ScaleFree=0,
            ErdosReyni,
            numGraphTypes
        };
        static std::string getGraphTypeName(GraphType type);
        class Conf
        {
        public:
            static double convertPerDayCountToInterArrivalDelayInMillisecs(size_t perDay);
            static double getErdosReyniEdgeProb(size_t numVertices, size_t numEdges);
        public:
            Conf()
                : graphType_(GraphType::ErdosReyni),
                  numVertices_(1000*1000),
                  numEdges_(100*numVertices_),
                  popularityZipfParam_(1.0),
                  popularityGroupCount_(100),
                  interArrivalTimeMean_(Conf::convertPerDayCountToInterArrivalDelayInMillisecs(10*numVertices_))
            {}
            GraphType & graphType() { return graphType_; }
            size_t & numVertices() { return numVertices_; }
            size_t & numEdges() { return numEdges_; }
            double & popularityZipfParam() { return popularityZipfParam_; }
            size_t & popularityGroupCount() { return popularityGroupCount_; }
            double & interArrivalTimeMean() { return interArrivalTimeMean_; }
        private:
            GraphType graphType_;
            size_t numVertices_;
            size_t numEdges_;
            double popularityZipfParam_;
            size_t popularityGroupCount_;
            double interArrivalTimeMean_;
        };
        struct VertexInfo
        {
            double time;
            size_t popularityIndex;
            size_t currentPopularityIndex;
        };
    public:
        Conf & conf() { return conf_; }
        void buildGraph();
        VertexIterator getVertexIterator();
        core::Edge generateNextEdge();
        core::VertexId getRandomPopularVertex();
    private:
        core::VertexId getFromVertex();
        core::VertexId getToVertex(core::VertexId from);
        void readjustPriorities(core::VertexId from, core::VertexId to);
    private:
        Conf conf_;
        double time_;
        boost::mt19937 rgenForExp_;
        boost::minstd_rand rgenForUnif_;
        typedef boost::variate_generator<boost::mt19937&, boost::exponential_distribution<>> ExpVariate;
        typedef boost::variate_generator<boost::minstd_rand&, boost::uniform_real<>> UnifVariate;
        std::auto_ptr<ExpVariate> expDist_;
        std::auto_ptr<UnifVariate> unifDist_;
        std::auto_ptr<Graph> graph_;
        GraphIndexMap vertexIndex_;
        std::auto_ptr<Zipf> popularitySkew_;
        std::vector<std::vector<core::VertexId>> verticesByPopularity_;
        std::tr1::unordered_map<core::VertexId, VertexInfo> vertexInfo_;
    };

} } /* namespace */


#endif /* INTERGEN_INTERACTIONGRAPHGENERATOR_H */
