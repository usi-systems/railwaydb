#ifndef INTERGB_INTERVALQUERYINDEX_H
#define INTERGB_INTERVALQUERYINDEX_H

#include <intergdb/core/Block.h>
#include <intergdb/core/BlockManager.h>
#include <intergdb/core/Conf.h>
#include <intergdb/core/Exceptions.h>
#include <intergdb/core/RTreeIntervalIndex.h>
#include <intergdb/core/Types.h>
#include <intergdb/core/NetworkByteBuffer.h>

#include <memory>
#include <tr1/memory>

namespace intergdb { namespace core
{
    template <class EdgeData>
    class IntervalQueryIndex
    {
    public:
        class Iterator
        {
        public:
            Iterator(RTreeIntervalIndex & iidx, BlockManager<EdgeData> * bman,
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
            BlockManager<EdgeData> * bman_;
            std::auto_ptr<RTreeIntervalIndex::Iterator> iidxIter_;
        };
    public:
        IntervalQueryIndex(Conf const & conf, BlockManager<EdgeData> * bman);
        void indexBlock(Block<EdgeData> const & block);
        std::tr1::shared_ptr<Iterator> query(Timestamp start, Timestamp end);
        void queryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results);
    private:
        BlockManager<EdgeData> * bman_;
        RTreeIntervalIndex iidx_;
    };

    #define INTERVAL_QUERY_INDEX_NAME "iq_index"

    template<class EdgeData>
    IntervalQueryIndex<EdgeData>::IntervalQueryIndex(Conf const & conf, BlockManager<EdgeData> * bman)
        : bman_(bman)
    {
        iidx_.openOrCreate(conf.getStorageDir()+"/"+INTERVAL_QUERY_INDEX_NAME);
    }

    #undef INTERVAL_QUERY_INDEX_NAME

    template<class EdgeData>
    IntervalQueryIndex<EdgeData>::Iterator::Iterator(RTreeIntervalIndex & iidx,
            BlockManager<EdgeData> * bman, Timestamp start, Timestamp end)
        : start_(start), end_(end), bman_(bman)
    {
        iidxIter_.reset(iidx.getNewIterator(start, end));
        findNext();
    }

    template<class EdgeData>
    bool IntervalQueryIndex<EdgeData>::IntervalQueryIndex::Iterator::isValid()
    {
        return iidxIter_->isValid();
    }

    template<class EdgeData>
    void IntervalQueryIndex<EdgeData>::IntervalQueryIndex::Iterator::next()
    {
        assert(isValid());
        iidxIter_->moveToNext();
        findNext();
    }

    template<class EdgeData>
    void IntervalQueryIndex<EdgeData>::IntervalQueryIndex::Iterator::findNext()
    {
        while (isValid()) {
            readCurrents();
            if (currentEnd_<start_ || currentStart_>=end_) {
                iidxIter_->moveToNext();
                continue; // no match (index may return a superset)
            }
            // we have to read the block if the range is within the neighbor list
            if (start_>currentStart_ && end_<=currentEnd_) {
                Block<EdgeData> const & block = bman_->getBlock(currentBlock_);
                assert(block.getNeighborLists().count(currentVertex_)>0);
                auto const & nlist = block.getNeighborLists().find(currentVertex_)->second;
                if (!nlist.hasEdgesInRange(start_, end_))
                    iidxIter_->moveToNext();
                else break;
            } else break;
        }
    }

    template<class EdgeData>
    void IntervalQueryIndex<EdgeData>::IntervalQueryIndex::Iterator::readCurrents()
    {
        assert(iidxIter_->isValid());
        currentEnd_ = iidxIter_->getData().getIntervalEnd();
        currentStart_ = iidxIter_->getData().getIntervalStart();
        currentVertex_ = iidxIter_->getData().getVertex();
        currentBlock_ = iidxIter_->getData().getBlockId();
    }

    template<class EdgeData>
    std::tr1::shared_ptr<typename IntervalQueryIndex<EdgeData>::Iterator> IntervalQueryIndex<EdgeData>::
        query(Timestamp start, Timestamp end)
    {
        return std::tr1::shared_ptr<IntervalQueryIndex::Iterator>(new Iterator(iidx_, bman_, start, end));
    }

    template<class EdgeData>
    void IntervalQueryIndex<EdgeData>::queryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results)
    {
        iidx_.queryBatch(start, end, results);
    }

    template <class EdgeData>
    void IntervalQueryIndex<EdgeData>::indexBlock(Block<EdgeData> const & block)
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
