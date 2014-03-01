#ifndef INTERGDB_RUN_EXP_EXPSETUPHELPER_H
#define INTERGDB_RUN_EXP_EXPSETUPHELPER_H

#include <intergdb/run/AutoTimer.h>
#include <intergdb/core/InteractionGraph.h>
#include <intergdb/core/Types.h>
#include <intergdb/gen/InteractionGraphGenerator.h>
#include <functional>

namespace intergdb { namespace run { namespace exp
{
    class ExpSetupHelper
    {
    private:
        static std::string getString(size_t size);
    public:
        static void setupGraphGeneratorWithDefaults(gen::InteractionGraphGenerator::Conf & conf);
        static void clearDBFilesForTheExperiment(std::string const & expName);
        static core::Conf createGraphConfWithDefaults(std::string const & expName);
        static void populateGraphFromTweets(std::string const & dirPath,
                core::InteractionGraph<int64_t, std::string> & graph,
                size_t * nVertices=nullptr, size_t * nEdges=nullptr);
        static void scanTweets(std::string const & dirPath,
                std::function<void (uint64_t, int64_t, std::vector<int64_t> const &,
                                    std::string const &, std::string const &)> visitor);

        template <class VertexData, class EdgeData>
        static void addGraphVertices(gen::InteractionGraphGenerator & gen,
                                     core::InteractionGraph<VertexData,EdgeData> & graph,
                                     size_t dataSize, size_t * nVertices=nullptr)
        {
            size_t numVertices = 0;
            for (auto vit=gen.getVertexIterator(); vit.isValid(); vit.next()) {
                graph.createVertex(vit.getVertex(), ExpSetupHelper::getString(dataSize));
                numVertices++;
            }
            if (nVertices!=nullptr)
                *nVertices = numVertices;
        }
        template <class VertexData, class EdgeData>
        static std::pair<double,double> addGraphEdges(
                gen::InteractionGraphGenerator & gen,
                core::InteractionGraph<VertexData,EdgeData> & graph,
                double duration, size_t dataSize, size_t * nEdges=nullptr)
        {
            size_t minute = 0;
            double minTime=0.0, maxTime=0.0;
            auto edge = gen.generateNextEdge();
            minTime = maxTime = edge.getTime();
            graph.addEdge(edge.getFromVertex(), edge.getToVertex(),
                          ExpSetupHelper::getString(dataSize), edge.getTime());
            size_t numEdges = 1;
            while (maxTime-minTime<duration) {
                edge = gen.generateNextEdge();
                maxTime = edge.getTime();
                graph.addEdge(edge.getFromVertex(), edge.getToVertex(),
                              ExpSetupHelper::getString(dataSize), maxTime);
                numEdges++;
                auto cminute = (size_t)(maxTime/(1000*60));
                if (cminute>minute) {
                    minute = cminute;
                    std::clog << minute << " minutes of data is generated, (" << numEdges <<  " edges)" << std::endl;
                }
            }
            if (nEdges!=nullptr)
                 (*nEdges) = numEdges;
            return std::make_pair(minTime,maxTime);
        }
        template <class VertexData, class EdgeData>
        static size_t doVertexQueries(
                core::InteractionGraph<VertexData,EdgeData> & graph,
                double minTime, double maxTime, double timeExtent,
                size_t numQueriesToRun, std::vector<core::VertexId> & vertices)
        {
            core::PriorityQueue<size_t, core::VertexId,
                core::PriorityQueueOrder::MaxAtTop> popularVertices(numQueriesToRun);
            size_t numResults = 0;
            double timeDiff = maxTime - minTime;
            double timeDelta = (timeDiff - timeExtent) / numQueriesToRun;
            double time = minTime + (timeExtent + timeDelta) / 2.0;
            for (size_t i=0; i<numQueriesToRun; i++, time+=timeDelta) {
                core::Timestamp startTime = time - timeExtent/2.0;
                core::Timestamp endTime = time +  timeExtent/2.0;
                auto iqIt = graph.processIntervalQuery(startTime, endTime);
                for (; iqIt.isValid(); iqIt.next()) {
                    auto id = iqIt.getVertexId();
                    //int64_t data = *iqIt.getVertexData(); // this is costly the first time (vertex data cache not there yet)
                    size_t count = 0;
                    if (popularVertices.hasItem(id))
                        count = popularVertices.getItemKey(id);
                    popularVertices.updateItem(count+1, id);
                    numResults++;
                }
            }
            while (!popularVertices.empty()) {
                auto id = popularVertices.getTopItem().id();
                vertices.push_back(id);
                popularVertices.removeItem(id);
            }
            return numResults;
        }

        template <class VertexData, class EdgeData>
        static size_t doEdgeQueries(
                core::InteractionGraph<VertexData,EdgeData> & graph,
                double minTime, double maxTime, double timeExtent,
                size_t numQueriesToRun, std::vector<core::VertexId> const & vertices)
        {
            size_t numResults = 0;
            double timeDiff = maxTime - minTime;
            double timeDelta = (timeDiff - timeExtent) / numQueriesToRun;
            double time = minTime + (timeExtent + timeDelta) / 2.0;
            for (size_t i=0; i<numQueriesToRun; i++, time+=timeDelta) {
                core::Timestamp startTime = time - timeExtent/2.0;
                core::Timestamp endTime = time +  timeExtent/2.0;
                core::VertexId vertex = (i<vertices.size()) ? vertices[i] : 0;
                auto fiqIt = graph.processFocusedIntervalQuery(vertex, startTime, endTime);
                for (; fiqIt.isValid(); fiqIt.next()) {
                    std::string data = *fiqIt.getEdgeData();
                    auto to = fiqIt.getToVertex();
                    auto tm  = fiqIt.getTime();
                    (void) to; (void) tm;
                    numResults++;
                }
            }
            return numResults;
        }

        template <class VertexData, class EdgeData>
        static size_t doNHopEdgeQueries(
                core::InteractionGraph<VertexData,EdgeData> & graph,
                double minTime, double maxTime, double timeExtent, size_t nHops,
                size_t numQueriesToRun, std::vector<core::VertexId> const & vertices)
        {
            size_t numResults = 0;
            double timeDiff = maxTime - minTime;
            double timeDelta = (timeDiff - timeExtent) / numQueriesToRun;
            double time = minTime + (timeExtent + timeDelta) / 2.0;
            for (size_t i=0; i<numQueriesToRun; i++, time+=timeDelta) {
                core::Timestamp startTime = time - timeExtent/2.0;
                core::Timestamp endTime = time +  timeExtent/2.0;
                core::VertexId headVertex = (i<vertices.size()) ? vertices[i] : 0;
                std::unordered_set<core::VertexId> seenVertices;
                seenVertices.insert(headVertex);
                std::deque<std::pair<core::VertexId, int>> queue;
                queue.push_back(std::make_pair(headVertex, 0));
                while (!queue.empty()) {
                    int hop = queue.front().second;
                    auto vertex = queue.front().first;
                    queue.pop_front();
                    auto fiqIt = graph.processFocusedIntervalQuery(vertex, startTime, endTime);
                    for (; fiqIt.isValid(); fiqIt.next()) {
                        std::string data = *fiqIt.getEdgeData();
                        auto to = fiqIt.getToVertex();
                        auto tm  = fiqIt.getTime();
                        (void) to; (void) tm;
                        if (hop+1<nHops && seenVertices.count(to)==0)
                            queue.push_back(std::make_pair(to, hop+1));
                        seenVertices.insert(to);
                        numResults++;
                    }
                }
            }
            return numResults;
        }

        template <class VertexData, class EdgeData>
        static size_t doNHopEdgeQueriesForTweets(
                core::InteractionGraph<VertexData,EdgeData> & graph,
                double minTime, double maxTime, double timeExtent, size_t nHops,
                size_t numQueriesToRun, std::vector<core::VertexId> const & vertices)
        {
            size_t numResults = 0;
            double timeDiff = maxTime - minTime;
            double timeDelta = (timeDiff - timeExtent) / numQueriesToRun;
            double time = minTime + (timeExtent + timeDelta) / 2.0;
            for (size_t i=0; i<numQueriesToRun; i++, time+=timeDelta) {
                core::Timestamp startTime = time - timeExtent/2.0;
                core::Timestamp endTime = time +  timeExtent/2.0;
                core::VertexId headVertex = (i<vertices.size()) ? vertices[i] : 0;
                std::unordered_set<core::VertexId> seenVertices;
                seenVertices.insert(headVertex);
                std::deque<std::pair<core::VertexId, int>> queue;
                queue.push_back(std::make_pair(headVertex, 0));
                while (!queue.empty()) {
                    int hop = queue.front().second;
                    auto vertex = queue.front().first;
                    queue.pop_front();
                    auto fiqIt = graph.processFocusedIntervalQuery(vertex, startTime, endTime);
                    for (; fiqIt.isValid(); fiqIt.next()) {
                        std::string data = *fiqIt.getEdgeData();
                        auto other = fiqIt.getToVertex();
                        auto tm  = fiqIt.getTime();
                        (void) tm;
                        assert(data[0]=='s'||data[0]=='l');
                        core::VertexId from = (data[0]=='s')
                                ? std::min((int64_t)vertex, (int64_t)other)
                                : std::max((int64_t)vertex, (int64_t)other);
                        if (from!=vertex)
                            continue;
                        if (hop+1<nHops && seenVertices.count(other)==0)
                            queue.push_back(std::make_pair(other, hop+1));
                        seenVertices.insert(other);
                        numResults++;
                    }
                }
            }
            return numResults;
        }

        template <class VertexData, class EdgeData>
        static void doBFSQueries(
                core::InteractionGraph<VertexData,EdgeData> & graph,
                double minTime, double maxTime, double timeExtent, size_t nHops,
                size_t numQueriesToRun, std::vector<core::VertexId> const & vertices)
        {
            double timeDiff = maxTime - minTime;
            double time = minTime + timeDiff / 2.0;
            for (size_t i=0; i<numQueriesToRun; i++) {
                core::Timestamp startTime = time - timeExtent/2.0;
                core::Timestamp endTime = time +  timeExtent/2.0;
                core::VertexId headVertex = vertices[i%vertices.size()];
                std::unordered_set<core::VertexId> seenVertices;
                seenVertices.insert(headVertex);
                std::deque<std::pair<core::VertexId, int>> queue;
                queue.push_back(std::make_pair(headVertex, 0));
                while (!queue.empty()) {
                    int hop = queue.front().second;
                    auto vertex = queue.front().first;
                    queue.pop_front();
                    auto fiqIt = graph.processFocusedIntervalQuery(vertex, startTime, endTime);
                    for (; fiqIt.isValid(); fiqIt.next()) {
                        std::string data = *fiqIt.getEdgeData();
                        auto to = fiqIt.getToVertex();
                        auto tm  = fiqIt.getTime();
                        (void) to; (void) tm;
                        if (hop+1<nHops && seenVertices.count(to)==0)
                            queue.push_back(std::make_pair(to, hop+1));
                        seenVertices.insert(to);
                    }
                }
            }
        }

        template<class T>
        size_t intersectionSize(std::unordered_set<T> const & s1,
                                std::unordered_set<T> const & s2)
        {
            std::unordered_set<T> const * s = nullptr;
            std::unordered_set<T> const * l = nullptr;
            if (s1.size() <= s2.size()) {
                s = &s1;
                l = &s2;
            } else {
                s = &s2;
                l = &s1;
            }
            size_t c = 0;
            for (T const & i : *s)
                if (l->count(i)>0)
                    c++;
            return c;
        }

        template <class VertexData, class EdgeData>
        static void doClusteringCoefficientQueries(
                core::InteractionGraph<VertexData,EdgeData> & graph,
                double minTime, double maxTime, double timeExtent,
                size_t numQueriesToRun, std::vector<core::VertexId> const & vertices)
        {
            double timeDiff = maxTime - minTime;
            double time = minTime + timeDiff / 2.0;
            for (size_t i=0; i<numQueriesToRun; i++) {
                core::Timestamp startTime = time - timeExtent/2.0;
                core::Timestamp endTime = time +  timeExtent/2.0;
                core::VertexId v = vertices[i%vertices.size()];
                double clusteringCoefficient = 0.0;
                std::unordered_set<core::VertexId> tz;
                auto fiqIt = graph.processFocusedIntervalQuery(v, startTime, endTime);
                for (; fiqIt.isValid(); fiqIt.next()) {
                    auto to = fiqIt.getToVertex();
                    tz.insert(to);
                }
                for (core::VertexId const & id : tz) {
                    auto fiqIt = graph.processFocusedIntervalQuery(id, startTime, endTime);
                    for (; fiqIt.isValid(); fiqIt.next()) {
                        auto to = fiqIt.getToVertex();
                        if (tz.count(to)>0)
                            clusteringCoefficient++;
                    }
                }
                size_t z = tz.size();
                clusteringCoefficient /= z*(z-1);
            }
        }

        // random walk
        template <class VertexData, class EdgeData>
        static void doRandomWalkQueries(
                core::InteractionGraph<VertexData,EdgeData> & graph,
                double minTime, double maxTime, double timeExtent, size_t nHops,
                size_t numQueriesToRun, std::vector<core::VertexId> const & vertices)
        {
            double timeDiff = maxTime - minTime;
            double time = minTime + timeDiff / 2.0;
            for (size_t i=0; i<numQueriesToRun; i++) {
                core::Timestamp startTime = time - timeExtent/2.0;
                core::Timestamp endTime = time +  timeExtent/2.0;
                core::VertexId v = vertices[i%vertices.size()];
                for (size_t n=0; n<nHops; ++n) {
                    auto fiqIt = graph.processFocusedIntervalQuery(v, startTime, endTime);
                    std::vector<core::VertexId> neigs;
                    for (; fiqIt.isValid(); fiqIt.next()) {
                        auto to = fiqIt.getToVertex();
                        neigs.push_back(to);
                    }
                    if (neigs.size()!=0)
                        v = neigs[(rand()/(RAND_MAX+1.0))*neigs.size()];
                }
            }
        }

        // page rank: one step
        template <class VertexData, class EdgeData>
        static void doPageRankQueries(
                core::InteractionGraph<VertexData,EdgeData> & graph,
                double minTime, double maxTime, double timeExtent, size_t nIters)
        {
            double timeDiff = maxTime - minTime;
            double time = minTime + timeDiff / 2.0;
            core::Timestamp startTime = time - timeExtent/2.0;
            core::Timestamp endTime = time +  timeExtent/2.0;
            std::unordered_set<core::VertexId> pages;
            auto iqIt = graph.processIntervalQuery(startTime, endTime);
            for (; iqIt.isValid(); iqIt.next()) {
                auto to = iqIt.getVertexId();
                pages.insert(to);
            }
            double damp = 0.85;
            std::unordered_map<core::VertexId,double> ranks1, ranks2;
            for (auto const & v : pages)
                ranks1[v] = 1.0/pages.size();
            for (size_t k=0; k<nIters; k++) {
                for (auto const & v : pages)
                    ranks2[v] = 0.0;
            for (core::VertexId const & from : pages) {
                std::unordered_map<core::VertexId,double> delta;
                auto fiqIt = graph.processFocusedIntervalQuery(from, startTime, endTime);
                size_t nEdges = 0;
                for (; fiqIt.isValid(); fiqIt.next()) {
                    auto to = fiqIt.getToVertex();
                    delta[to] += ranks1[from];
                    nEdges++;
                }
                for (std::pair<core::VertexId,double> const & item : delta)
                    ranks2[item.first] += item.second / nEdges;
            }
            for (auto const & v : pages)
                ranks2[v] = (1.0-damp)/pages.size() + damp*ranks2[v];
            ranks1.swap(ranks2);
            }
        }
    };
} } }

#endif /* INTERGDB_RUN_EXP_EXPSETUPHELPER_H */
