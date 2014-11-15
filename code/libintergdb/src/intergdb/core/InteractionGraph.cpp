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

InteractionGraph::VertexIterator InteractionGraph::processIntervalQuery(Timestamp start, Timestamp end)
{
    return VertexIterator(&vman_, hisg_.intervalQuery(start, end));
}

void InteractionGraph::processIntervalQueryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results)
{
    return hisg_.intervalQueryBatch(start, end, results);
}

InteractionGraph::EdgeIterator InteractionGraph::processFocusedIntervalQuery(VertexId vertex, Timestamp start, Timestamp end)
{
    return EdgeIterator(hisg_.focusedIntervalQuery(vertex, start, end));
}