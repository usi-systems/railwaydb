#include <intergdb/core/NeighborList.h>

#include <unordered_map>
#include <algorithm>

using namespace std;
using namespace intergdb::core;

bool NeighborList::getEdgeAttributeData(
    VertexId to, Timestamp tm, std::shared_ptr<AttributeData> & sdata)
{
    bool found = false;
    Edge tmEdge(to, tm, std::shared_ptr<AttributeData>());
    auto it = std::lower_bound(edges_.begin(), edges_.end(), tmEdge);
    for (; it!=edges_.end() && it->getTime()==tm; ++it) {
        if (it->getToVertex()==to) {
            sdata = it->getData();
            found = true;
            break;
        }
    }
    assert(!found || (it->getToVertex()==to && it->getTime()==tm));
    return found;
}

bool NeighborList::hasEdgesInRange(Timestamp start, Timestamp end) const
{
    Edge startEdge(0, start, std::shared_ptr<AttributeData>());
    auto it = std::lower_bound(edges_.begin(), edges_.end(), startEdge);
    if (it==edges_.end())
        return false;
    if (it->getTime()>=end)
        return false;
    return true;
}
