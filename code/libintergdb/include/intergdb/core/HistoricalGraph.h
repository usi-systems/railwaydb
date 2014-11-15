#pragma once

#include <intergdb/core/BlockManager.h>
#include <intergdb/core/IntervalQueryIndex.h>
#include <intergdb/core/FocusedIntervalQueryIndex.h>

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
                         BlockManager * bman);
            bool isValid();
            void next();
            typename NeighborList::Edge const & getEdge();
        private:
            void initFromBlock();
        private:
            bool done_;
            size_t currentEdgeIndex_;
            size_t currentNumEdges_;
            BlockManager * bman_;
            std::shared_ptr<FocusedIntervalQueryIndex::Iterator> it_;
        };
    public:
        HistoricalGraph(Conf const & conf);
        void addBlock(Block & block);
        std::shared_ptr<VertexIterator> intervalQuery(Timestamp start, Timestamp end);
        void intervalQueryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results);
        std::shared_ptr<EdgeIterator> focusedIntervalQuery(VertexId vertex, Timestamp start, Timestamp end);
        size_t getEdgeIOCount() const { return bman_.getNumIOReads() + bman_.getNumIOWrites(); }
        size_t getEdgeReadIOCount() const { return bman_.getNumIOReads(); }
        size_t getEdgeWriteIOCount() const { return bman_.getNumIOWrites(); }
    private:
        BlockManager bman_;
        IntervalQueryIndex iqIndex_;
        FocusedIntervalQueryIndex fiqIndex_;
    };

} } /* namespace */
