#ifndef INTERGDB_INTERACTIONGRAPH_H
#define INTERGDB_INTERACTIONGRAPH_H

#include <intergdb/core/Conf.h>
#include <intergdb/core/Types.h>
#include <intergdb/core/Helper.h>
#include <intergdb/core/VertexManager.h>
#include <intergdb/core/InMemoryGraph.h>
#include <intergdb/core/HistoricalGraph.h>
#include <intergdb/core/AttributeData.h>
#include <intergdb/core/Schema.h>

#include <memory>
#include <utility>

namespace intergdb { namespace core
{
    template<class VertexData>
    class InteractionGraph
    {
    public:
        class VertexIterator
        {
        public:
            VertexIterator(VertexManager<VertexData> * vman,
              std::shared_ptr<typename HistoricalGraph::VertexIterator> it)
                : vman_(vman), it_(it) {}
            bool isValid() { return it_->isValid(); }
            void next() { it_->next(); }
            VertexId getVertexId() { return it_->getVertexId(); }
            std::shared_ptr<VertexData> getVertexData()
                { return vman_->getVertexData(getVertexId()); }
        private:
            VertexManager<VertexData> * vman_;
            std::shared_ptr<typename HistoricalGraph::VertexIterator> it_;
        };
        class EdgeIterator
        {
        public:
            EdgeIterator(std::shared_ptr<typename HistoricalGraph::EdgeIterator> it)
                : it_(it) {}
            bool isValid() { return it_->isValid(); }
            void next() { it_->next(); }
            VertexId getToVertex() { return it_->getEdge().getToVertex(); }
            Timestamp getTime() { return it_->getEdge().getTime(); }
            std::shared_ptr<AttributeData> getEdgeData() { return it_->getEdge().getData(); }
        private:
            std::shared_ptr<typename HistoricalGraph::EdgeIterator> it_;
        };
    public:
        InteractionGraph(Conf const & conf);

        void createVertex(VertexId id, VertexData const & data);
        std::shared_ptr<VertexData> getVertexData(VertexId id);
        // TODO: The same vertex cannot be involved in more than one edge with the same timestamp
        // This limitation can be removed with some additional work
        template <typename... EdgeDataAttributes>
        void addEdge(VertexId v, VertexId u, 
                     Timestamp time /*=Helper::getCurrentTimestamp() */,
                     EdgeDataAttributes&&... edgeData);
        void flush();
        VertexIterator processIntervalQuery(Timestamp start, Timestamp end);
        EdgeIterator processFocusedIntervalQuery(VertexId headVertex, Timestamp start, Timestamp end);
        void processIntervalQueryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results);
        BlockStats const & getBlockStats() const { return memg_.getBlockStats(); }
        size_t getEdgeIOCount() const { return hisg_.getEdgeIOCount(); }
        size_t getEdgeReadIOCount() const { return hisg_.getEdgeReadIOCount(); }
        size_t getEdgeWriteIOCount() const { return hisg_.getEdgeReadIOCount().getEdgeWriteIOCount(); }
        Schema & getSchema() { return schema_; }
    private:
        Conf conf_;
        VertexManager<VertexData> vman_;
        HistoricalGraph hisg_;
        InMemoryGraph memg_;
        Schema schema_;
    };

    template<class VertexData>
    InteractionGraph<VertexData>::InteractionGraph(Conf const & conf)
      : conf_(conf), vman_(conf_), hisg_(conf_, schema_), memg_(conf_, &hisg_, schema_) { }

    template<class VertexData>
    void InteractionGraph<VertexData>::
        createVertex(VertexId id, VertexData const & data)
    {
        vman_.addVertex(id, data);
    }

    template<class VertexData>
    void InteractionGraph<VertexData>::flush()
    {
        memg_.flush();
    }

    template<class VertexData>
    std::shared_ptr<VertexData> InteractionGraph<VertexData>::
        getVertexData(VertexId id)
    {
        return vman_.getVertexData(id);
    }

    template<typename T1, typename... TN>
    struct AttributeCollector
    {
      static void add(AttributeData * data, int index, T1&& t1, TN&&... tn) 
      {
        data->setAttribute(index, t1);
        AttributeCollector<TN...>::add(data, index+1, std::forward<TN>(tn)...);
      }
    };

    template<typename T>
    struct AttributeCollector<T>
    {
      static void add(AttributeData * data, int index, T&& t) 
      {
        data->setAttribute(index, t);
      }
    };

    template<class VertexData>
    template <typename... EdgeDataAttributes>
    void InteractionGraph<VertexData>::
        addEdge(VertexId v, VertexId u, 
                Timestamp time,/*=Helper::getCurrentTimestamp()*/
                EdgeDataAttributes&&... edgeData)
    {
        assert(v!=u);
        getVertexData(v);
        getVertexData(u);
        AttributeData * data = getSchema().newAttributeData();
        AttributeCollector<EdgeDataAttributes...>::
            add(data, 0, std::forward<EdgeDataAttributes>(edgeData)...);
        memg_.addEdge(v, u, time, data);
    }

    template<class VertexData>
    typename InteractionGraph<VertexData>::VertexIterator InteractionGraph<VertexData>::
        processIntervalQuery(Timestamp start, Timestamp end)
    {
        return VertexIterator(&vman_, hisg_.intervalQuery(start, end));
    }

    template<class VertexData>
    void InteractionGraph<VertexData>::
        processIntervalQueryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results)
    {
        return hisg_.intervalQueryBatch(start, end, results);
    }

    template<class VertexData>
    typename InteractionGraph<VertexData>::EdgeIterator InteractionGraph<VertexData>::
        processFocusedIntervalQuery(VertexId vertex, Timestamp start, Timestamp end)
    {
        return EdgeIterator(hisg_.focusedIntervalQuery(vertex, start, end));
    }
} } /* namespace */

#endif /* INTERGDB_INTERACTIONGRAPH_H */
