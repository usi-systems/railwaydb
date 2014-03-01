#ifndef INTERGDB_INMEMORYGRAPH_H
#define INTERGDB_INMEMORYGRAPH_H

#include <intergdb/core/Conf.h>
#include <intergdb/core/Types.h>
#include <intergdb/core/NeighborList.h>
#include <intergdb/core/VertexFIFO.h>
#include <intergdb/core/HistoricalGraph.h>
#include <intergdb/core/ExpirationMap.h>

#include <deque>
#include <memory>
#include <tr1/memory>
#include <tr1/unordered_map>
#include <cassert>

namespace intergdb { namespace core
{
    template<class EdgeData>
    class InMemoryGraph
    {
    public:
        InMemoryGraph(Conf const & conf, HistoricalGraph<EdgeData> * hisg);
        void addEdge(VertexId from, VertexId to, Timestamp ts, EdgeData const & data);
        void flush();
        BlockStats const & getBlockStats() const { return expm_.getBlockStats(); }
    private:
        std::tr1::shared_ptr<EdgeData> removeEdge(UEdge const & edge);
    private:
        VertexFIFO vfifo_;
        ExpirationMap<EdgeData> expm_;
        std::tr1::unordered_map<VertexId, NeighborList<EdgeData> > neigLists_;
    };

    template<class EdgeData>
    InMemoryGraph<EdgeData>::InMemoryGraph(Conf const & conf, HistoricalGraph<EdgeData> * hisg)
      : vfifo_(conf.windowSize()), expm_(conf, hisg) {}

    template<class EdgeData>
    void InMemoryGraph<EdgeData>::addEdge(VertexId v, VertexId u,
                                          Timestamp ts, EdgeData const & data)
    {
        typedef typename NeighborList<EdgeData>::Edge NLEdge;
        std::tr1::shared_ptr<EdgeData> sdata(new EdgeData(data));
        {
            NeighborList<EdgeData> & nlist = neigLists_[v];
            nlist.headVertex() = v;
            nlist.addEdge(NLEdge(u, ts, sdata));
        }
        {
            NeighborList<EdgeData> & nlist = neigLists_[u];
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
            std::tr1::shared_ptr<EdgeData> data = removeEdge(oldEdge);
            expm_.addEdge(oldEdge, data);
            vfifo_.popOldestEdge();
        }
    }

    template<class EdgeData>
    void InMemoryGraph<EdgeData>::flush()
    {
        while(!vfifo_.isEmpty()) {
            UEdge & oldEdge = vfifo_.getOldestEdge();
            std::tr1::shared_ptr<EdgeData> data = removeEdge(oldEdge);
            expm_.addEdge(oldEdge, data);
            vfifo_.popOldestEdge();
        }
        expm_.flush();
    }

    template<class EdgeData>
    std::tr1::shared_ptr<EdgeData> InMemoryGraph<EdgeData>::removeEdge(UEdge const & edge)
    {
        std::tr1::shared_ptr<EdgeData> data;
        {
            VertexId v = edge.getFirstVertex();
            NeighborList<EdgeData> & nlist = neigLists_[v];
            assert(nlist.getOldestEdge().getToVertex()==edge.getSecondVertex());
            assert(nlist.getOldestEdge().getTime()==edge.getTime());
            data = nlist.getOldestEdge().getData();
            nlist.removeOldestEdge();
            if (nlist.getEdges().size()==0)
                neigLists_.erase(v);
        }
        {
            VertexId u = edge.getSecondVertex();
            NeighborList<EdgeData> & nlist = neigLists_[u];
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
