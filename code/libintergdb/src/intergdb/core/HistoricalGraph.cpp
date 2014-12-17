#include <intergdb/core/HistoricalGraph.h>

#include <intergdb/core/Conf.h>

using namespace std;
using namespace intergdb::core;

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
    EdgeIterator(std::shared_ptr<FocusedIntervalQueryIndex::Iterator> it,
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
        auto const & nlist = bman_->getBlock(it_->getBlocks()[0]).getNeighborLists().find(it_->getVertex())->second;
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
    // TODO: update needed for handling partitions
    auto const & nlist = bman_->getBlock(it_->getBlocks()[0]).getNeighborLists().find(it_->getVertex())->second;
    return nlist.getNthOldestEdge(currentEdgeIndex_);
}

void HistoricalGraph::EdgeIterator::initFromBlock()
{
    auto const & nlist = bman_->getBlock(it_->getBlocks()[0]).getNeighborLists().find(it_->getVertex())->second;
    currentNumEdges_ = nlist.getEdges().size();
    for (currentEdgeIndex_=0; currentEdgeIndex_<currentNumEdges_; currentEdgeIndex_++)
        if (nlist.getNthOldestEdge(currentEdgeIndex_).getTime()>=it_->getRangeStartTime())
            break;
    assert(currentEdgeIndex_<currentNumEdges_);
    if (nlist.getNthOldestEdge(currentEdgeIndex_).getTime()>=it_->getRangeEndTime())
        done_ = true;
}

HistoricalGraph::HistoricalGraph(Conf const & conf, PartitionIndex & pidx)
    : conf_(conf), bman_(conf), pidx_(pidx), iqIndex_(conf, &bman_), fiqIndex_(conf)
{}

void HistoricalGraph::addBlock(Block & block)
{
    Timestamp minTimestamp, maxTimestamp;
    block.findMinMaxTimestamps(minTimestamp, maxTimestamp);
    Timestamp blockTimestamp = (minTimestamp+maxTimestamp)/2;
    TimeSlicedPartitioning tpart = pidx_.getTimeSlicedPartitioning(blockTimestamp);
    Partitioning const & part = tpart.getPartitioning();
    vector<Block> subBlocks = block.partitionBlock(part, conf_.getEdgeSchema());
    for (Block & subBlock : subBlocks) 
        bman_.addBlock(subBlock); // sets the block id        
    iqIndex_.indexBlock(subBlocks[0]);
    fiqIndex_.indexBlocks(subBlocks);
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