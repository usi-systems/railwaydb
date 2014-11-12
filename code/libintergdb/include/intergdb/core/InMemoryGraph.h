#ifndef INTERGDB_INMEMORYGRAPH_H
#define INTERGDB_INMEMORYGRAPH_H

#include <intergdb/core/Conf.h>
#include <intergdb/core/Types.h>
#include <intergdb/core/NeighborList.h>
#include <intergdb/core/VertexFIFO.h>
#include <intergdb/core/HistoricalGraph.h>
#include <intergdb/core/ExpirationMap.h>
#include <intergdb/core/EdgeData.h>

#include <deque>
#include <memory>
#include <unordered_map>
#include <cassert>

namespace intergdb { namespace core
{
    class InMemoryGraph
    {
    public:
        InMemoryGraph(Conf const & conf, HistoricalGraph * hisg);
        void addEdge(VertexId from, VertexId to, Timestamp ts, EdgeData * data);
        void flush(Schema & schema);
        BlockStats const & getBlockStats() const { return expm_.getBlockStats(); }
    private:
        std::shared_ptr<EdgeData> removeEdge(UEdge const & edge);
    private:
        VertexFIFO vfifo_;
        ExpirationMap expm_;
        std::unordered_map<VertexId, NeighborList > neigLists_;
    };

    InMemoryGraph::InMemoryGraph(Conf const & conf, HistoricalGraph * hisg)
      : vfifo_(conf.windowSize()), expm_(conf, hisg) {}

    void InMemoryGraph::addEdge(VertexId v, VertexId u, Timestamp ts, EdgeData * data)
    {
        typedef NeighborList::Edge NLEdge;
        std::shared_ptr<EdgeData> sdata(data);
        {
            NeighborList & nlist = neigLists_[v];
            nlist.headVertex() = v;
            nlist.addEdge(NLEdge(u, ts, sdata));
        }
        {
            NeighborList & nlist = neigLists_[u];
            nlist.headVertex() = u;
            nlist.addEdge(NLEdge(v, ts, sdata));
        }
        UEdge edge(v, u, ts);
        vfifo_.addEdge(edge);
        //Timestamp lastExpired = 0;
        while(vfifo_.hasExpiredEdges() /*||
                (!vfifo_.isEmpty() && vfifo_.getOldestEdge().getTime()==lastExpired)*/) {
            UEdge & oldEdge = vfifo_.getOldestEdge();
            //lastExpired = oldEdge.getTime();
            std::shared_ptr<EdgeData> data = removeEdge(oldEdge);
            expm_.addEdge(oldEdge, data);
            vfifo_.popOldestEdge();
        }
    }

    void InMemoryGraph::flush(Schema & schema)
    {
        while(!vfifo_.isEmpty()) {
            UEdge & oldEdge = vfifo_.getOldestEdge();
            std::shared_ptr<EdgeData> data = removeEdge(oldEdge);
            expm_.addEdge(oldEdge, data);
            vfifo_.popOldestEdge();
        }
        expm_.flush(schema);
    }

    std::shared_ptr<EdgeData> InMemoryGraph::removeEdge(UEdge const & edge)
    {
        std::shared_ptr<EdgeData> data;
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

} } /* namespace */

#endif /* INTERGDB_INMEMORYGRAPH_H */
