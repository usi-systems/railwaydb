#include <intergdb/core/HistoricalGraph.h>

#include <intergdb/core/Conf.h>
#include <intergdb/common/Query.h>
#include <intergdb/core/MetaDataManager.h>
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

HistoricalGraph::EdgeIterator::EdgeIterator(
    std::shared_ptr<FocusedIntervalQueryIndex::Iterator> it,
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
        auto const & nlist = bman_->getBlock(
            it_->getBlockId()).getNeighborLists().find(
                it_->getVertex())->second;
        if (nlist.getNthOldestEdge(currentEdgeIndex_).getTime() >=
            it_->getRangeEndTime())
            done_ = true;
    } else {
        it_->next();
        if (it_->isValid())
            initFromBlock();
    }
}

NeighborList::Edge const & HistoricalGraph::EdgeIterator::getEdge()
{
    auto const & nlist = bman_->getBlock(
        it_->getBlockId()).getNeighborLists().find(it_->getVertex())->second;
    return nlist.getNthOldestEdge(currentEdgeIndex_);
}

shared_ptr<AttributeData> HistoricalGraph::EdgeIterator::getEdgeData()
{
    Schema const & schema = bman_->getEdgeSchema();
    shared_ptr<AttributeData> data(schema.newAttributeData(queryAttributes_));
    Block masterBlock = bman_->getBlock(it_->getBlockId());
    assert(masterBlock.partition_ == -1);
    assert(masterBlock.getSubBlockIds().size() > 0);
    auto const & subBlocks = masterBlock.getSubBlockIds();
    for (auto const & index : partitionIndices_) {
        assert(index < subBlocks.size());
        auto const & nlist = bman_->getBlock(subBlocks[index])
            .getNeighborLists().find(it_->getVertex())->second;
        data->setAttributes(
            *nlist.getNthOldestEdge(currentEdgeIndex_).getData());
    }
    return data;
}

size_t HistoricalGraph::EdgeIterator::getAttributeSize(
    string const & attr) const
{
    Schema const & schema = bman_->getEdgeSchema();
    SchemaStats const & stats = bman_->getEdgeSchemaStats();
    return stats.getAvgSize(schema.getIndex(attr));
}

size_t HistoricalGraph::EdgeIterator::getPartitionSize(
    unordered_set<string> const & attrs) const
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
    while (selectedAttributes.size() != queryAttributes_.size()) {
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
    Block block = bman_->getBlock(it_->getBlockId()); // a copy
    Timestamp partitioningTimestamp = block.getPartitioningTimestamp();
    if (partitionIndices_.empty()) {
        partitioning_ = bman_->getPartitionIndex()
            .getTimeSlicedPartitioning(partitioningTimestamp);
        recomputePartitionIndices();
    } else {
        if(partitioningTimestamp < partitioning_.getStartTime() ||
           partitioningTimestamp >= partitioning_.getEndTime()) {
            partitioning_ = bman_->getPartitionIndex()
                .getTimeSlicedPartitioning(partitioningTimestamp);
            recomputePartitionIndices();
        }
    }
    auto const & nlist = block.getNeighborLists().find(
        it_->getVertex())->second;
    currentNumEdges_ = nlist.getEdges().size();
    for (currentEdgeIndex_=0; currentEdgeIndex_ < currentNumEdges_;
        currentEdgeIndex_++)
    {
        if (nlist.getNthOldestEdge(currentEdgeIndex_).getTime() >=
            it_->getRangeStartTime())
            break;
    }
    assert(currentEdgeIndex_<currentNumEdges_);
    if (nlist.getNthOldestEdge(currentEdgeIndex_).getTime() >=
        it_->getRangeEndTime())
    {
        done_ = true;
    }
}

HistoricalGraph::HistoricalGraph(Conf const & conf, MetaDataManager & meta)
    : conf_(conf), partIndex_(conf), bman_(conf, partIndex_, meta),
    iqIndex_(conf, &bman_), fiqIndex_(conf)
{
    partIndex_.setIntervalQueryIndex(&iqIndex_);
}

void HistoricalGraph::addBlock(Block const & block)
{
    vector<Block> newBlocks = block.partitionBlock(
        conf_.getEdgeSchema(), partIndex_);
    assert(newBlocks.size()>1);
    for (size_t i=1, iu=newBlocks.size(); i<iu; ++i) {
        bman_.addBlock(newBlocks[i]); // sets the block id
        newBlocks[0].addSubBlockId(newBlocks[i].id());
    }
    bman_.addBlock(newBlocks[0]);
    iqIndex_.indexBlock(newBlocks[0]);
    fiqIndex_.indexBlock(newBlocks[0]);
}

std::shared_ptr<HistoricalGraph::VertexIterator> HistoricalGraph::
    intervalQuery(Timestamp start, Timestamp end)
{
    return std::shared_ptr<VertexIterator>(
        new VertexIterator(iqIndex_.query(start, end)));
}

void HistoricalGraph::intervalQueryBatch(
    Timestamp start, Timestamp end, std::vector<VertexId> & results)
{
    iqIndex_.queryBatch(start, end, results);
}

void intervalQueryBatch(Timestamp start, Timestamp end,
                        std::vector<VertexId> & results);

std::shared_ptr<HistoricalGraph::EdgeIterator>
    HistoricalGraph::focusedIntervalQuery(FocusedIntervalQuery const &  query)
{
    return std::shared_ptr<EdgeIterator>(
        new EdgeIterator(fiqIndex_.query(query.getHeadVertex(),
                         query.getStartTime(), query.getEndTime()),
        &bman_, query));
}
