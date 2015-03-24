#include <intergdb/core/IntervalQueryIndex.h>

#include <intergdb/core/Block.h>
#include <intergdb/core/BlockManager.h>
#include <intergdb/core/Conf.h>
#include <intergdb/core/Exceptions.h>
#include <intergdb/core/NetworkByteBuffer.h>

using namespace std;
using namespace intergdb::core;

#define INTERVAL_QUERY_INDEX_NAME "iq_index"

IntervalQueryIndex::IntervalQueryIndex(Conf const & conf, BlockManager * bman)
    : bman_(bman)
{
    iidx_.openOrCreate(conf.getStorageDir()+"/"+INTERVAL_QUERY_INDEX_NAME);
}

IntervalQueryIndex::Iterator::Iterator(
    RTreeIntervalIndex & iidx, BlockManager * bman,
    Timestamp start, Timestamp end)
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
            auto const & nlist = block.getNeighborLists().find(
                currentVertex_)->second;
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
    return std::shared_ptr<IntervalQueryIndex::Iterator>(
        new Iterator(iidx_, bman_, start, end));
}

void IntervalQueryIndex::queryBatch(Timestamp start, Timestamp end,
                                    std::vector<VertexId> & results)
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
