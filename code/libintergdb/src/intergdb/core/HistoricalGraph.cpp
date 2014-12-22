#include <intergdb/core/HistoricalGraph.h>

#include <intergdb/core/Conf.h>
#include <intergdb/common/Query.h>
#include <intergdb/common/SystemConstants.h>

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
                 BlockManager * bman, FocusedIntervalQuery const & query)
    : done_(false), bman_(bman), it_(it),
      queryAttributes_(query.getAttributeNames().begin(), 
        query.getAttributeNames().end()) 
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

NeighborList::Edge const & HistoricalGraph::EdgeIterator::getEdge()
{
    auto const & nlist = bman_->getBlock(it_->getBlocks()[0]).getNeighborLists().find(it_->getVertex())->second;
    return nlist.getNthOldestEdge(currentEdgeIndex_);
}

shared_ptr<AttributeData> HistoricalGraph::EdgeIterator::getEdgeData() 
{ 
    Schema const & schema = bman_->getEdgeSchema();
    shared_ptr<AttributeData> data(schema.newAttributeData(queryAttributes_));
    for (auto const & index : partitionIndices_) {
        auto const & nlist = bman_->getBlock(it_->getBlocks()[index]).getNeighborLists().find(it_->getVertex())->second;
        data->setAttributes(*nlist.getNthOldestEdge(currentEdgeIndex_).getData());
    }
    return data;
}

size_t HistoricalGraph::EdgeIterator::getAttributeSize(string const & attr) const
{
    Schema const & schema = bman_->getEdgeSchema();
    auto index = schema.getIndex(attr);
    return schema.getAttributes()[index].getSize();
}

size_t HistoricalGraph::EdgeIterator::getPartitionSize(unordered_set<string> const & attrs) const
{
    size_t totalSize = 0;
    for (auto const & attr : attrs)
        totalSize += getAttributeSize(attr);
    return totalSize;
}

void HistoricalGraph::EdgeIterator::recomputePartitionIndices()
{
    partitionIndices_.clear();
    Partitioning const & parting = partitioning_.getPartitioning();
    unordered_set<string> selectedAttributes; 
    unordered_set<size_t> unusedPartitions;
    for (size_t i=0; i<parting.size(); ++i) 
        unusedPartitions.insert(i);
    while (selectedAttributes.size()!=queryAttributes_.size()) {
        auto bestPartitionIdx = parting.size();
        double bestPartitionScore = -1.0;
        for (auto const & partitionIdx : unusedPartitions) {
            double partitionScore = 0.0;
            double partitionSize = getPartitionSize(parting[partitionIdx]);
            for (auto const & attr : parting[partitionIdx]) {
                if (selectedAttributes.count(attr)>0) 
                    continue; // already selected
                if (queryAttributes_.count(attr)==0) 
                    continue; // not in the query
                partitionScore += (getAttributeSize(attr) * 
                    SystemConstants::numberOfEdgesInABlock) / partitionSize;
            }
            if (partitionScore > bestPartitionScore) {
                bestPartitionScore = partitionScore;
                bestPartitionIdx = partitionIdx;
            }
        }
        for(auto const & attr : parting[bestPartitionIdx]) {
            if (queryAttributes_.count(attr)>0) 
                selectedAttributes.insert(attr); 
        }
        partitionIndices_.push_back(bestPartitionIdx);
        unusedPartitions.erase(bestPartitionIdx);
    }
}

void HistoricalGraph::EdgeIterator::initFromBlock()
{
    // TODO: ideally we should pin this block until we are done with it
    Block const & block = bman_->getBlock(it_->getBlocks()[0]); 
    if (partitionIndices_.empty()) {
        partitioning_ = bman_->getBlockPartitioning(block);
        recomputePartitionIndices();
    } else {   
        Timestamp minTimestamp, maxTimestamp;
        block.findMinMaxTimestamps(minTimestamp, maxTimestamp);
        Timestamp blockTimestamp = (minTimestamp+maxTimestamp)/2.0;
        if(blockTimestamp < partitioning_.getStartTime() || 
           blockTimestamp >= partitioning_.getEndTime()) {
            partitioning_ = bman_->getBlockPartitioning(block.id());
            recomputePartitionIndices();
        }
    }
    auto const & nlist = block.getNeighborLists().find(it_->getVertex())->second;
    currentNumEdges_ = nlist.getEdges().size();
    for (currentEdgeIndex_=0; currentEdgeIndex_<currentNumEdges_; currentEdgeIndex_++)
        if (nlist.getNthOldestEdge(currentEdgeIndex_).getTime()>=it_->getRangeStartTime())
            break;
    assert(currentEdgeIndex_<currentNumEdges_);
    if (nlist.getNthOldestEdge(currentEdgeIndex_).getTime()>=it_->getRangeEndTime())
        done_ = true;    
}

HistoricalGraph::HistoricalGraph(Conf const & conf, PartitionIndex & pidx)
    : conf_(conf), pidx_(pidx), bman_(conf, pidx_), iqIndex_(conf, &bman_), fiqIndex_(conf)
{}

void HistoricalGraph::addBlock(Block & block)
{
    vector<Block> subBlocks = block.partitionBlock(conf_.getEdgeSchema(), pidx_);
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

std::shared_ptr<HistoricalGraph::EdgeIterator> HistoricalGraph::focusedIntervalQuery(FocusedIntervalQuery const &  query)
{
    return std::shared_ptr<EdgeIterator>(new EdgeIterator(fiqIndex_.query(query.getHeadVertex(), query.getStartTime(), query.getEndTime()), &bman_, query));
}
