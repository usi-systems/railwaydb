#ifndef INTERGB_INTERVALQUERYINDEX_H
#define INTERGB_INTERVALQUERYINDEX_H

#include <intergdb/core/Block.h>
#include <intergdb/core/BlockManager.h>
#include <intergdb/core/Conf.h>
#include <intergdb/core/Exceptions.h>
#include <intergdb/core/RTreeIntervalIndex.h>
#include <intergdb/core/Types.h>
#include <intergdb/core/NetworkByteBuffer.h>
#include <intergdb/core/EdgeData.h>

#include <memory>

namespace intergdb { namespace core
{
    class IntervalQueryIndex
    {
    public:
        class Iterator
        {
        public:
            Iterator(RTreeIntervalIndex & iidx, BlockManager * bman,
                    Timestamp start, Timestamp end);
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

    #define INTERVAL_QUERY_INDEX_NAME "iq_index"

    IntervalQueryIndex::IntervalQueryIndex(Conf const & conf, BlockManager * bman)
        : bman_(bman)
    {
        iidx_.openOrCreate(conf.getStorageDir()+"/"+INTERVAL_QUERY_INDEX_NAME);
    }

    #undef INTERVAL_QUERY_INDEX_NAME

    IntervalQueryIndex::Iterator::Iterator(RTreeIntervalIndex & iidx,
            BlockManager * bman, Timestamp start, Timestamp end)
        : start_(start), end_(end), bman_(bman)
    {
        iidxIter_.reset(iidx.getNewIterator(start, end));
        findNext();
    }

    bool IntervalQueryIndex::IntervalQueryIndex::Iterator::isValid()
    {
        return iidxIter_->isValid();
    }

    void IntervalQueryIndex::IntervalQueryIndex::Iterator::next()
    {
        assert(isValid());
        iidxIter_->moveToNext();
        findNext();
    }

    void IntervalQueryIndex::IntervalQueryIndex::Iterator::findNext()
    {
        while (isValid()) {
            readCurrents();
            if (currentEnd_<start_ || currentStart_>=end_) {
                iidxIter_->moveToNext();
                continue; // no match (index may return a superset)
            }
            // we have to read the block if the range is within the neighbor list
            if (start_>currentStart_ && end_<=currentEnd_) {
                Block const & block = bman_->getBlock(currentBlock_);
                assert(block.getNeighborLists().count(currentVertex_)>0);
                auto const & nlist = block.getNeighborLists().find(currentVertex_)->second;
                if (!nlist.hasEdgesInRange(start_, end_))
                    iidxIter_->moveToNext();
                else break;
            } else break;
        }
    }

    void IntervalQueryIndex::IntervalQueryIndex::Iterator::readCurrents()
    {
        assert(iidxIter_->isValid());
        currentEnd_ = iidxIter_->getData().getIntervalEnd();
        currentStart_ = iidxIter_->getData().getIntervalStart();
        currentVertex_ = iidxIter_->getData().getVertex();
        currentBlock_ = iidxIter_->getData().getBlockId();
    }

    std::shared_ptr<typename IntervalQueryIndex::Iterator> IntervalQueryIndex::
        query(Timestamp start, Timestamp end)
    {
        return std::shared_ptr<IntervalQueryIndex::Iterator>(new Iterator(iidx_, bman_, start, end));
    }

    void IntervalQueryIndex::queryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results)
    {
        iidx_.queryBatch(start, end, results);
    }

    void IntervalQueryIndex::indexBlock(Block const & block)
    {
        auto const & nlists = block.getNeighborLists();
        for (auto it=nlists.begin(); it!=nlists.end(); ++it) {
            auto const & nlist = it->second;
            VertexId head = nlist.headVertex();
            assert(head==it->first);
            Timestamp start = nlist.getOldestEdge().getTime();
            Timestamp end = nlist.getNewestEdge().getTime();
            iidx_.addInterval(block.id(), head, start, end);
        }
    }
} }

#endif /* INTERGB_INTERVALQUERYINDEX_H */
