#ifndef HISTORICAL_INMEMORYGRAPH_H
#define HISTORICAL_INMEMORYGRAPH_H

#include <intergdb/core/Conf.h>
#include <intergdb/core/BlockManager.h>
#include <intergdb/core/IntervalQueryIndex.h>
#include <intergdb/core/FocusedIntervalQueryIndex.h>

#include <tr1/unordered_set>

namespace intergdb { namespace core
{

template<class EdgeData>
class HistoricalGraph
{
public:
    // we may have duplicates, so we need to filter those out
    class VertexIterator
    {
    public:
        VertexIterator(std::tr1::shared_ptr<typename IntervalQueryIndex<EdgeData>::Iterator> it);
        bool isValid();
        void next();
        VertexId getVertexId();
    private:
        VertexId last_;
        std::tr1::unordered_set<VertexId> vertices_;
        std::tr1::shared_ptr<typename IntervalQueryIndex<EdgeData>::Iterator> it_;
    };
    class EdgeIterator
    {
    public:
        EdgeIterator(std::tr1::shared_ptr<typename FocusedIntervalQueryIndex<EdgeData>::Iterator > it,
                     BlockManager<EdgeData> * bman);
        bool isValid();
        void next();
        typename NeighborList<EdgeData>::Edge const & getEdge();
    private:
        void initFromBlock();
    private:
        bool done_;
        size_t currentEdgeIndex_;
        size_t currentNumEdges_;
        BlockManager<EdgeData> * bman_;
        std::tr1::shared_ptr<typename FocusedIntervalQueryIndex<EdgeData>::Iterator> it_;
    };
public:
    HistoricalGraph(Conf const & conf);
    void addBlock(Block<EdgeData> & block);
    std::tr1::shared_ptr<VertexIterator> intervalQuery(Timestamp start, Timestamp end);
    void intervalQueryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results);
    std::tr1::shared_ptr<EdgeIterator> focusedIntervalQuery(VertexId vertex, Timestamp start, Timestamp end);
    size_t getEdgeIOCount() const { return bman_.getNumIOReads() + bman_.getNumIOWrites(); }
    size_t getEdgeReadIOCount() const { return bman_.getNumIOReads(); }
    size_t getEdgeWriteIOCount() const { return bman_.getNumIOWrites(); }
private:
    BlockManager<EdgeData> bman_;
    IntervalQueryIndex<EdgeData> iqIndex_;
    FocusedIntervalQueryIndex<EdgeData> fiqIndex_;
};

template<class EdgeData>
HistoricalGraph<EdgeData>::VertexIterator::
    VertexIterator(std::tr1::shared_ptr<typename IntervalQueryIndex<EdgeData>::Iterator> it)
  : it_(it)
{
    if (it_->isValid()) {
        last_ = it_->getVertex();
        vertices_.insert(last_);
    }
}

template<class EdgeData>
bool HistoricalGraph<EdgeData>::VertexIterator::isValid()
{
    return it_->isValid();
}

template<class EdgeData>
void HistoricalGraph<EdgeData>::VertexIterator::next()
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

template<class EdgeData>
VertexId HistoricalGraph<EdgeData>::VertexIterator::getVertexId()
{
    return last_;
}

template<class EdgeData>
HistoricalGraph<EdgeData>::EdgeIterator::
    EdgeIterator(std::tr1::shared_ptr<typename FocusedIntervalQueryIndex<EdgeData>::Iterator > it,
                 BlockManager<EdgeData> * bman)
    : done_(false), bman_(bman), it_(it)
{
    if (it_->isValid())
        initFromBlock();
}

template<class EdgeData>
bool HistoricalGraph<EdgeData>::EdgeIterator::isValid()
{
    return it_->isValid() && !done_;
}

template<class EdgeData>
void HistoricalGraph<EdgeData>::EdgeIterator::next()
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

template<class EdgeData>
typename NeighborList<EdgeData>::Edge const & HistoricalGraph<EdgeData>::EdgeIterator::
     getEdge()
{
    auto const & nlist = bman_->getBlock(it_->getBlock()).getNeighborLists().find(it_->getVertex())->second;
    return nlist.getNthOldestEdge(currentEdgeIndex_);
}

template<class EdgeData>
void HistoricalGraph<EdgeData>::EdgeIterator::initFromBlock()
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

template<class EdgeData>
HistoricalGraph<EdgeData>::HistoricalGraph(Conf const & conf)
  : bman_(conf), iqIndex_(conf, &bman_), fiqIndex_(conf)
{}

template<class EdgeData>
void HistoricalGraph<EdgeData>::addBlock(Block<EdgeData> & block)
{
    bman_.addBlock(block); // sets the block id
    iqIndex_.indexBlock(block);
    fiqIndex_.indexBlock(block);
}

template<class EdgeData>
std::tr1::shared_ptr<typename HistoricalGraph<EdgeData>::VertexIterator> HistoricalGraph<EdgeData>::
    intervalQuery(Timestamp start, Timestamp end)
{
    return std::tr1::shared_ptr<VertexIterator>(new VertexIterator(iqIndex_.query(start, end)));
}

template<class EdgeData>
void HistoricalGraph<EdgeData>::
    intervalQueryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results)
{
    iqIndex_.queryBatch(start, end, results);
}

void intervalQueryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results);

template<class EdgeData>
std::tr1::shared_ptr<typename HistoricalGraph<EdgeData>::EdgeIterator> HistoricalGraph<EdgeData>::
    focusedIntervalQuery(VertexId vertex, Timestamp start, Timestamp end)
{
    return std::tr1::shared_ptr<EdgeIterator>(new EdgeIterator(fiqIndex_.query(vertex, start, end), &bman_));
}

} } /* namespace */

#endif /* INTERGDB_HISTORICALGRAPH_H */
