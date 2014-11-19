#include <intergdb/core/InteractionGraph.h>

using namespace std;
using namespace intergdb::core;

InteractionGraph::InteractionGraph(Conf const & conf)
    : conf_(conf), vman_(conf_), hisg_(conf_), memg_(conf_, &hisg_) { }

void InteractionGraph::flush()
{
    memg_.flush();
}

std::shared_ptr<AttributeData> InteractionGraph::getVertexData(VertexId id)
{
    return vman_.getVertexData(id);
}

InteractionGraph::VertexIterator InteractionGraph::processIntervalQuery(Query q)
{
    return VertexIterator(&vman_, hisg_.intervalQuery(q.getStart(), q.getEnd()));
}

void InteractionGraph::processIntervalQueryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results)
{
    return hisg_.intervalQueryBatch(start, end, results);
}

InteractionGraph::EdgeIterator InteractionGraph::processFocusedIntervalQuery(FocusedIntervalQuery q)
{
    return EdgeIterator(hisg_.focusedIntervalQuery(q.getHeadVertex(), q.getStart(), q.getEnd()));
}
