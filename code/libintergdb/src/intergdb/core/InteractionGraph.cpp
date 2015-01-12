#include <intergdb/core/InteractionGraph.h>

using namespace std;
using namespace intergdb::core;

InteractionGraph::InteractionGraph(Conf const & conf)
    : conf_(conf), vman_(conf_), pidx_(conf_), hisg_(conf_, pidx_, stats_), memg_(conf_, &hisg_, stats_), qcol_(conf_), meta_(conf_.getStorageDir()) { }

void InteractionGraph::flush()
{
    memg_.flush();
    meta_.store(stats_);
}

std::shared_ptr<AttributeData> InteractionGraph::getVertexData(VertexId id)
{
    return vman_.getVertexData(id);
}

InteractionGraph::VertexIterator InteractionGraph::processIntervalQuery(IntervalQuery const & q)
{
    return VertexIterator(&vman_, hisg_.intervalQuery(q.getStartTime(), q.getEndTime()));
}

void InteractionGraph::processIntervalQueryBatch(IntervalQuery const & q, std::vector<VertexId> & results)
{
    return hisg_.intervalQueryBatch(q.getStartTime(), q.getEndTime(), results);
}

InteractionGraph::EdgeIterator InteractionGraph::processFocusedIntervalQuery(FocusedIntervalQuery const & q)
{
    qcol_.collectFocusedIntervalQuery(q);
    return EdgeIterator(hisg_.focusedIntervalQuery(q));
}
