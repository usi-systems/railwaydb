#ifndef INTERGDB_INTERACTIONGRAPH_H
#define INTERGDB_INTERACTIONGRAPH_H

#include <intergdb/core/Conf.h>
#include <intergdb/core/Types.h>
#include <intergdb/core/Helper.h>
#include <intergdb/core/VertexManager.h>
#include <intergdb/core/InMemoryGraph.h>
#include <intergdb/core/HistoricalGraph.h>

#include <tr1/memory>

namespace intergdb { namespace core
{
    template<class VertexData, class EdgeData>
    class InteractionGraph
    {
    public:
        class VertexIterator
        {
        public:
            VertexIterator(VertexManager<VertexData> * vman,
              std::tr1::shared_ptr<typename HistoricalGraph<EdgeData>::VertexIterator> it)
                : vman_(vman), it_(it) {}
            bool isValid() { return it_->isValid(); }
            void next() { it_->next(); }
            VertexId getVertexId() { return it_->getVertexId(); }
            std::tr1::shared_ptr<VertexData> getVertexData()
                { return vman_->getVertexData(getVertexId()); }
        private:
            VertexManager<VertexData> * vman_;
            std::tr1::shared_ptr<typename HistoricalGraph<EdgeData>::VertexIterator> it_;
        };
        class EdgeIterator
        {
        public:
            EdgeIterator(std::tr1::shared_ptr<typename HistoricalGraph<EdgeData>::EdgeIterator> it)
                : it_(it) {}
            bool isValid() { return it_->isValid(); }
            void next() { it_->next(); }
            VertexId getToVertex() { return it_->getEdge().getToVertex(); }
            Timestamp getTime() { return it_->getEdge().getTime(); }
            std::tr1::shared_ptr<EdgeData> getEdgeData() { return it_->getEdge().getData(); }
        private:
            std::tr1::shared_ptr<typename HistoricalGraph<EdgeData>::EdgeIterator> it_;
        };
    public:
        InteractionGraph(Conf const & conf);

        void createVertex(VertexId id, VertexData const & data);
        std::tr1::shared_ptr<VertexData> getVertexData(VertexId id);
        // The same vertex cannot be involved in more than one edge with the same timestamp
        void addEdge(VertexId v, VertexId u, EdgeData const & data,
                     Timestamp time=Helper::getCurrentTimestamp());

        void flush();
        VertexIterator processIntervalQuery(Timestamp start, Timestamp end);
        EdgeIterator processFocusedIntervalQuery(VertexId headVertex, Timestamp start, Timestamp end);
        void processIntervalQueryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results);
        BlockStats const & getBlockStats() const { return memg_.getBlockStats(); }
        size_t getEdgeIOCount() const { return hisg_.getEdgeIOCount(); }
        size_t getEdgeReadIOCount() const { return hisg_.getEdgeReadIOCount(); }
        size_t getEdgeWriteIOCount() const { return hisg_.getEdgeReadIOCount.getEdgeWriteIOCount(); }
    private:
        Conf conf_;
        VertexManager<VertexData> vman_;
        HistoricalGraph<EdgeData> hisg_;
        InMemoryGraph<EdgeData> memg_;
    };

    template<class VertexData, class EdgeData>
    InteractionGraph<VertexData,EdgeData>::InteractionGraph(Conf const & conf)
      : conf_(conf), vman_(conf_), hisg_(conf_), memg_(conf_, &hisg_) {}

    template<class VertexData, class EdgeData>
    void InteractionGraph<VertexData,EdgeData>::
        createVertex(VertexId id, VertexData const & data)
    {
        vman_.addVertex(id, data);
    }

    template<class VertexData, class EdgeData>
    void InteractionGraph<VertexData,EdgeData>::flush()
    {
        memg_.flush();
    }

    template<class VertexData, class EdgeData>
    std::tr1::shared_ptr<VertexData> InteractionGraph<VertexData,EdgeData>::
        getVertexData(VertexId id)
    {
        return vman_.getVertexData(id);
    }

    template<class VertexData, class EdgeData>
    void InteractionGraph<VertexData,EdgeData>::
        addEdge(VertexId v, VertexId u, EdgeData const & data,
                Timestamp time/*=Helper::getCurrentTimestamp()*/)
    {
        assert(v!=u);
        getVertexData(v);
        getVertexData(u);
        memg_.addEdge(v, u, time, data);
    }

    template<class VertexData, class EdgeData>
    typename InteractionGraph<VertexData,EdgeData>::VertexIterator InteractionGraph<VertexData,EdgeData>::
        processIntervalQuery(Timestamp start, Timestamp end)
    {
        return VertexIterator(&vman_, hisg_.intervalQuery(start, end));
    }

    template<class VertexData, class EdgeData>
    void InteractionGraph<VertexData,EdgeData>::
        processIntervalQueryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results)
    {
        return hisg_.intervalQueryBatch(start, end, results);
    }

    template<class VertexData, class EdgeData>
    typename InteractionGraph<VertexData,EdgeData>::EdgeIterator InteractionGraph<VertexData,EdgeData>::
        processFocusedIntervalQuery(VertexId vertex, Timestamp start, Timestamp end)
    {
        return EdgeIterator(hisg_.focusedIntervalQuery(vertex, start, end));
    }
} } /* namespace */

#endif /* INTERGDB_INTERACTIONGRAPH_H */
