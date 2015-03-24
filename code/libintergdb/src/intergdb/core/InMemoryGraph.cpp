#include <intergdb/core/InMemoryGraph.h>

#include <intergdb/core/AttributeData.h>
#include <intergdb/core/Conf.h>
#include <intergdb/core/HistoricalGraph.h>

using namespace std;
using namespace intergdb::core;

InMemoryGraph::InMemoryGraph(
    Conf const & conf, HistoricalGraph * hisg, SchemaStats & stats)
    : vfifo_(conf.windowSize()), expm_(conf, hisg), stats_(stats)
{}

void InMemoryGraph::addEdge(VertexId v, VertexId u, Timestamp ts,
                            std::shared_ptr<AttributeData> data)
{
    typedef NeighborList::Edge NLEdge;
    {
        NeighborList & nlist = neigLists_[v];
        nlist.headVertex() = v;
        nlist.addEdge(NLEdge(u, ts, data));
    }
    {
        NeighborList & nlist = neigLists_[u];
        nlist.headVertex() = u;
        nlist.addEdge(NLEdge(v, ts, data));
    }
    UEdge edge(v, u, ts);
    vfifo_.addEdge(edge);
    //Timestamp lastExpired = 0;
    while(vfifo_.hasExpiredEdges()) /* ||
      (!vfifo_.isEmpty() && vfifo_.getOldestEdge().getTime()==lastExpired)*/
    {
        UEdge & oldEdge = vfifo_.getOldestEdge();
        //lastExpired = oldEdge.getTime();
        std::shared_ptr<AttributeData> data = removeEdge(oldEdge);
        expm_.addEdge(oldEdge, data);
        vfifo_.popOldestEdge();
    }
    // hook to keep track of data sizes
    {
        for (auto const & indexTypePair : data->getFields()) {
            stats_.incrCountAndBytes(indexTypePair.first,
                                     data->getFieldSize(indexTypePair.first));
        }
    }
}

void InMemoryGraph::flush()
{
    while(!vfifo_.isEmpty()) {
        UEdge & oldEdge = vfifo_.getOldestEdge();
        std::shared_ptr<AttributeData> data = removeEdge(oldEdge);
        expm_.addEdge(oldEdge, data);
        vfifo_.popOldestEdge();
    }
    expm_.flush();
}

std::shared_ptr<AttributeData> InMemoryGraph::removeEdge(UEdge const & edge)
{
    std::shared_ptr<AttributeData> data;
    {
        VertexId v = edge.getFirstVertex();
        NeighborList & nlist = neigLists_[v];
        assert(nlist.getOldestEdge().getToVertex()==edge.getSecondVertex());
        assert(nlist.getOldestEdge().getTime()==edge.getTime());
        data = nlist.getOldestEdge().getData();
        nlist.removeOldestEdge();
        if (nlist.getEdges().size()==0)
            neigLists_.erase(v);
    }
    {
        VertexId u = edge.getSecondVertex();
        NeighborList & nlist = neigLists_[u];
        assert(nlist.getOldestEdge().getToVertex()==edge.getFirstVertex());
        assert(nlist.getOldestEdge().getTime()==edge.getTime());
        nlist.removeOldestEdge();
        if (nlist.getEdges().size()==0)
            neigLists_.erase(u);
    }
    return data;
}
