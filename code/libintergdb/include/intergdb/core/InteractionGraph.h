#pragma once

#include <intergdb/common/Types.h>


#include <intergdb/core/AttributeData.h>
#include <intergdb/core/Conf.h>
#include <intergdb/core/Helper.h>
#include <intergdb/core/HistoricalGraph.h>
#include <intergdb/core/InMemoryGraph.h>
#include <intergdb/core/MetaDataManager.h>
#include <intergdb/core/PartitionIndex.h>
#include <intergdb/core/Query.h>
#include <intergdb/core/QueryCollector.h>
#include <intergdb/core/Schema.h>
#include <intergdb/core/VertexManager.h>

#include <memory>
#include <utility>

namespace intergdb { namespace core
{
    class InteractionGraph
    {
    public:
        class VertexIterator
        {
        public:
            VertexIterator(VertexManager * vman, std::shared_ptr<typename HistoricalGraph::VertexIterator> it) : vman_(vman), it_(it) {}
            bool isValid() { return it_->isValid(); }
            void next() { it_->next(); }
            VertexId getVertexId() { return it_->getVertexId(); }
            std::shared_ptr<AttributeData> getVertexData() { return vman_->getVertexData(getVertexId()); }
        private:
            VertexManager * vman_;
            std::shared_ptr<typename HistoricalGraph::VertexIterator> it_;
        };
        class EdgeIterator
        {
        public:
            EdgeIterator(std::shared_ptr<typename HistoricalGraph::EdgeIterator> it) : it_(it) {}
            bool isValid() { return it_->isValid(); }
            void next() { it_->next(); }
            VertexId getToVertex() { return it_->getEdge().getToVertex(); }
            Timestamp getTime() { return it_->getEdge().getTime(); }
            std::shared_ptr<AttributeData> getEdgeData() { return it_->getEdgeData(); }
        private:
            std::shared_ptr<typename HistoricalGraph::EdgeIterator> it_;
        };
    public:
        InteractionGraph(Conf const & conf);
        template <typename... VertexDataAttributes>
        void createVertex(VertexId id, VertexDataAttributes&&... vertexData);
        std::shared_ptr<AttributeData> getVertexData(VertexId id);
        // TODO: The same vertex cannot be involved in more than one edge with the same timestamp
        // This limitation can be removed with some additional work
        template <typename... EdgeDataAttributes>
        void addEdge(VertexId v, VertexId u, 
                     Timestamp time /*=Helper::getCurrentTimestamp() */,
                     EdgeDataAttributes&&... edgeData);
        void flush();
        VertexIterator processIntervalQuery(IntervalQuery const & q);
        void processIntervalQueryBatch(IntervalQuery const & q, std::vector<VertexId> & results);
        EdgeIterator processFocusedIntervalQuery(FocusedIntervalQuery const & q);
        BlockStats const & getBlockStats() const { return memg_.getBlockStats(); }
        PartitionIndex & getPartitionIndex() { return hisg_.getPartitionIndex(); }
        size_t getEdgeIOCount() const { return hisg_.getEdgeIOCount(); }
        size_t getEdgeReadIOCount() const { return hisg_.getEdgeReadIOCount(); }
        size_t getEdgeWriteIOCount() const { return hisg_.getEdgeWriteIOCount(); }
        Schema const & getVertexSchema() { return conf_.getVertexSchema(); }
        Schema const & getEdgeSchema() { return conf_.getEdgeSchema(); }
        Conf const & getConf() const { return conf_; }
    private:
        Conf conf_;
        VertexManager vman_;
        PartitionIndex pidx_;
        MetaDataManager meta_;
        HistoricalGraph hisg_;
        InMemoryGraph memg_;
        QueryCollector qcol_;
    };

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

    template <typename... VertexDataAttributes>
    void InteractionGraph::
        createVertex(VertexId id, VertexDataAttributes&&... vertexData)
    {
        std::unique_ptr<AttributeData> data(getVertexSchema().newAttributeData());
        AttributeCollector<VertexDataAttributes...>::add(data.get(), 0, std::forward<VertexDataAttributes>(vertexData)...);
        vman_.addVertex(id, *data);
    }

    template <typename... EdgeDataAttributes>
    void InteractionGraph::addEdge(VertexId v, VertexId u, Timestamp time,/*=Helper::getCurrentTimestamp()*/ EdgeDataAttributes&&... edgeData)
    {
        assert(v!=u);
        getVertexData(v);
        getVertexData(u);
        std::shared_ptr<AttributeData> data(getEdgeSchema().newAttributeData());
        // TODO (rjs): Need to check that the edge added has the correct data for the schema
        if (sizeof...(EdgeDataAttributes) != getEdgeSchema().getAttributes().size())      
        {  
            throw std::runtime_error("Edge does not have the correct data for the schema.");
        }
        AttributeCollector<EdgeDataAttributes...>::add(data.get(), 0, std::forward<EdgeDataAttributes>(edgeData)...);
        memg_.addEdge(v, u, time, data);
    }
} } /* namespace */


