#ifndef HISTORICAL_INMEMORYGRAPH_H
#define HISTORICAL_INMEMORYGRAPH_H

#include <intergdb/core/Conf.h>
#include <intergdb/core/BlockManager.h>
#include <intergdb/core/IntervalQueryIndex.h>
#include <intergdb/core/FocusedIntervalQueryIndex.h>
#include <intergdb/core/EdgeData.h>

#include <unordered_set>

namespace intergdb { namespace core
{
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

HistoricalGraph::VertexIterator::
    VertexIterator(std::shared_ptr<IntervalQueryIndex::Iterator> it)
  : it_(it)
{
    if (it_->isValid()) {
        last_ = it_->getVertex();
        vertices_.insert(last_);
    }
}

bool HistoricalGraph::VertexIterator::isValid()
{
    return it_->isValid();
}

void HistoricalGraph::VertexIterator::next()
{
    assert(isValid());
    do {
        it_->next();
    } while(it_->isValid() && vertices_.count(it_->getVertex())>0);
    if (it_->isValid()) {
        last_ = it_->getVertex();
        vertices_.insert(last_);
    }
}

VertexId HistoricalGraph::VertexIterator::getVertexId()
{
    return last_;
}

HistoricalGraph::EdgeIterator::
    EdgeIterator(std::shared_ptr<FocusedIntervalQueryIndex::Iterator > it,
                 BlockManager * bman)
    : done_(false), bman_(bman), it_(it)
{
    if (it_->isValid())
        initFromBlock();
}

bool HistoricalGraph::EdgeIterator::isValid()
{
    return it_->isValid() && !done_;
}

void HistoricalGraph::EdgeIterator::next()
{
    assert(isValid());
    currentEdgeIndex_++;
    if (currentEdgeIndex_<currentNumEdges_) {
        auto const & nlist = bman_->getBlock(it_->getBlock()).getNeighborLists().find(it_->getVertex())->second;
        if(nlist.getNthOldestEdge(currentEdgeIndex_).getTime()>=it_->getRangeEndTime())
            done_ = true;
    } else {
        it_->next();
        if (it_->isValid())
            initFromBlock();
    }
}

NeighborList::Edge const & HistoricalGraph::EdgeIterator::
     getEdge()
{
    auto const & nlist = bman_->getBlock(it_->getBlock()).getNeighborLists().find(it_->getVertex())->second;
    return nlist.getNthOldestEdge(currentEdgeIndex_);
}

void HistoricalGraph::EdgeIterator::initFromBlock()
{
    auto const & nlist = bman_->getBlock(it_->getBlock()).getNeighborLists().find(it_->getVertex())->second;
    currentNumEdges_ = nlist.getEdges().size();
    for (currentEdgeIndex_=0; currentEdgeIndex_<currentNumEdges_; currentEdgeIndex_++)
        if (nlist.getNthOldestEdge(currentEdgeIndex_).getTime()>=it_->getRangeStartTime())
            break;
    assert(currentEdgeIndex_<currentNumEdges_);
    if (nlist.getNthOldestEdge(currentEdgeIndex_).getTime()>=it_->getRangeEndTime())
        done_ = true;
}

HistoricalGraph::HistoricalGraph(Conf const & conf)
  : bman_(conf), iqIndex_(conf, &bman_), fiqIndex_(conf)
{}

void HistoricalGraph::addBlock(Block & block)
{
    bman_.addBlock(block); // sets the block id
    iqIndex_.indexBlock(block);
    fiqIndex_.indexBlock(block);
}

std::shared_ptr<HistoricalGraph::VertexIterator> HistoricalGraph::
    intervalQuery(Timestamp start, Timestamp end)
{
    return std::shared_ptr<VertexIterator>(new VertexIterator(iqIndex_.query(start, end)));
}

void HistoricalGraph::intervalQueryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results)
{
    iqIndex_.queryBatch(start, end, results);
}

void intervalQueryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results);

std::shared_ptr<HistoricalGraph::EdgeIterator> HistoricalGraph::
    focusedIntervalQuery(VertexId vertex, Timestamp start, Timestamp end)
{
    return std::shared_ptr<EdgeIterator>(new EdgeIterator(fiqIndex_.query(vertex, start, end), &bman_));
}

} } /* namespace */

#endif /* INTERGDB_HISTORICALGRAPH_H */
