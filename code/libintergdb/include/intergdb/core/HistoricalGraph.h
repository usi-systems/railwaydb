#pragma once

#include <intergdb/core/BlockManager.h>
#include <intergdb/core/Conf.h>
#include <intergdb/core/Query.h>
#include <intergdb/core/IntervalQueryIndex.h>
#include <intergdb/core/FocusedIntervalQueryIndex.h>
#include <intergdb/core/PartitionIndex.h>

#include <memory>
#include <unordered_set>

namespace intergdb { namespace core
{
    class Conf;

    class HistoricalGraph
    {
    public:
        // we may have duplicates, so we need to filter those out
        class VertexIterator
        {
        public:
            VertexIterator(std::shared_ptr<IntervalQueryIndex::Iterator> it);
            bool isValid();
            void next();
            VertexId getVertexId();
        private:
            VertexId last_;
            std::unordered_set<VertexId> vertices_;
            std::shared_ptr<IntervalQueryIndex::Iterator> it_;
        };
        class EdgeIterator
        {
        public:
            EdgeIterator(std::shared_ptr<FocusedIntervalQueryIndex::Iterator > it,
                         BlockManager * bman, FocusedIntervalQuery const & query);
            bool isValid();
            void next();
            NeighborList::Edge const & getEdge();
            std::shared_ptr<AttributeData> getEdgeData();
        private:
            void initFromBlock();
            void recomputePartitionIndices();
            size_t getAttributeSize(std::string const & attr) const;
            size_t getPartitionSize(std::unordered_set<std::string> const & attrs) const;
        private:
            bool done_;
            size_t currentEdgeIndex_;
            size_t currentNumEdges_;
            TimeSlicedPartitioning partitioning_;
            std::vector<size_t> partitionIndices_;
            BlockManager * bman_;
            std::shared_ptr<FocusedIntervalQueryIndex::Iterator> it_;
            std::unordered_set<std::string> queryAttributes_;
        };
    public:
        HistoricalGraph(Conf const & conf, PartitionIndex & pidx, MetaDataManager & meta);
        void addBlock(Block const & block);
        std::shared_ptr<VertexIterator> intervalQuery(Timestamp start, Timestamp end);
        void intervalQueryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results);
        std::shared_ptr<EdgeIterator> focusedIntervalQuery(FocusedIntervalQuery const &  query);
        PartitionIndex & getPartitionIndex() { return partIndex_; }
        size_t getEdgeIOCount() const { return bman_.getNumIOReads() + bman_.getNumIOWrites(); }
        size_t getEdgeReadIOCount() const { return bman_.getNumIOReads(); }
        size_t getEdgeWriteIOCount() const { return bman_.getNumIOWrites(); }
    private:
        Conf conf_;
        PartitionIndex partIndex_;
        BlockManager bman_;        
        IntervalQueryIndex iqIndex_;
        FocusedIntervalQueryIndex fiqIndex_;
    };

} } /* namespace */
