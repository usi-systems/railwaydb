#pragma once

#include <intergdb/core/Types.h>
#include <intergdb/core/RTreeIntervalIndex.h>

#include <memory>

namespace intergdb { namespace core
{
    class Block;
    class BlockManager;
    class Conf;

    class IntervalQueryIndex
    {
    public:
        class Iterator
        {
        public:
            Iterator(RTreeIntervalIndex & iidx, BlockManager * bman, Timestamp start, Timestamp end);
            bool isValid();
            void next();
            Timestamp getEndTime() const { return currentEnd_; }
            VertexId getVertex() const { return currentVertex_; }
            Timestamp getStartTime() { return currentStart_; }
        private:
            void findNext();
            void readCurrents();
        private:
            Timestamp start_;
            Timestamp end_;
            // currents
            Timestamp currentStart_;
            Timestamp currentEnd_;
            VertexId currentVertex_;
            BlockId currentBlock_;
            BlockManager * bman_;
            std::auto_ptr<RTreeIntervalIndex::Iterator> iidxIter_;
        };
    public:
        IntervalQueryIndex(Conf const & conf, BlockManager * bman);
        void indexBlock(Block const & block);
        std::shared_ptr<Iterator> query(Timestamp start, Timestamp end);
        void queryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results);
    private:
        BlockManager * bman_;
        RTreeIntervalIndex iidx_;
    };

} }

