#ifndef INTERGDB_BLOCK_H
#define INTERGDB_BLOCK_H

#include <intergdb/core/Helper.h>
#include <intergdb/core/Types.h>
#include <intergdb/core/NeighborList.h>
#include <intergdb/core/NetworkByteBuffer.h>
#include <intergdb/core/EdgeData.h>

#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cassert>

namespace intergdb { namespace core
{
    class Block
    {
    public:
        typedef std::unordered_map<VertexId, NeighborList > NeighborListMap;
        Block() : id_(0), serializedSize_(sizeof(BlockId)) {}
        Block(BlockId id) : id_ (id), serializedSize_(sizeof(BlockId)) {}
        BlockId id() const { return id_; }
        BlockId & id() { return id_; }
        void addEdge(VertexId headVertex, VertexId to, Timestamp tm,
                    std::shared_ptr<EdgeData> data);
        void removeNewestEdge(VertexId headVertex);
        NeighborListMap const & getNeighborLists() const { return neigs_; }
        size_t getSerializedSize() const { return serializedSize_; }
        void swap(Block & other);
        std::ostream & print(std::ostream & out);
    private:
        void addNeighborList(VertexId id);
    private:
        BlockId id_;
        size_t serializedSize_;
        std::unordered_map<VertexId, NeighborList > neigs_;
    };

    void Block::swap(Block & other)
    {
        std::swap(id_, other.id_);
        std::swap(serializedSize_, other.serializedSize_);
        neigs_.swap(other.neigs_);
    }

    void Block::addNeighborList(VertexId id)
    {
        if (neigs_.count(id)==0) {
            neigs_[id].headVertex() = id;
            serializedSize_ += sizeof(VertexId);
            serializedSize_ += sizeof(uint32_t);
        }
    }

    void Block::addEdge(VertexId headVertex, VertexId to, Timestamp tm,
                                  std::shared_ptr<EdgeData> data)
    {
        std::shared_ptr<EdgeData> sdata;
        if (neigs_.count(to)>0) {
            auto & nlist = neigs_[to];
            bool there = nlist.getEdgeData(headVertex, tm, sdata);
            if (!(!there || (sdata.get()==data.get()))) {
                print(std::cerr) << std::endl;
            }
            assert(!there || (sdata.get()==data.get()));
        }
        if (sdata.get()==nullptr) {
            sdata = data;
            serializedSize_ += getSerializedSizeOf(*sdata);
        }
        addNeighborList(headVertex);
        auto & nlist = neigs_[headVertex];
        typedef typename NeighborList::Edge NLEdge;
        nlist.addEdge(NLEdge(to, tm, sdata));
        serializedSize_ += sizeof(VertexId);
        serializedSize_ += sizeof(Timestamp);
    }

    std::ostream & Block::print(std::ostream & out)
    {
        for (auto it=neigs_.begin(); it!=neigs_.end(); ++it) {
            VertexId headVertex = it->first;
            auto const & nlist = it->second;
            out << "Head vertex: " << headVertex << ", edges: ";
            size_t count = nlist.getEdges().size();
            for (size_t i=0; i<count; ++it) {
                auto const & edge = nlist.getNthOldestEdge(i);
                out << "(" << edge.getToVertex() << "," << edge.getTime() << "),";
            }
            out << "\n";
        }
        return out;
    }

    void Block::removeNewestEdge(VertexId headVertex)
    {
        assert(neigs_.count(headVertex)>0);
        NeighborList & nlist = neigs_[headVertex];
        auto const & edge = nlist.getNewestEdge();
        VertexId to = edge.getToVertex();
        if (neigs_.count(to)>0) {
            auto & nlist = neigs_[to];
            std::shared_ptr<EdgeData> sdata;
            bool there = nlist.getEdgeData(headVertex, edge.getTime(), sdata);
            if (!there)
                serializedSize_ -= getSerializedSizeOf(*edge.getData());
        }
        nlist.removeNewestEdge();
        serializedSize_ -= sizeof(Timestamp);
        serializedSize_ -= sizeof(VertexId);
        if (nlist.getEdges().empty()) {
            neigs_.erase(headVertex);
            serializedSize_ -= sizeof(uint32_t);
            serializedSize_ -= sizeof(VertexId);
        }
    }

    class EdgeTriplet
    {
    public:
        EdgeTriplet(VertexId id1, VertexId id2, Timestamp tm)
            : lowId_ (std::min(id1, id2)),
              highId_ (std::max(id1, id2)),
              tm_ (tm) {}
        VertexId getLowId() const { return lowId_; }
        VertexId getHighId() const { return highId_; }
        Timestamp getTime() const { return tm_; }
        bool operator==(EdgeTriplet const & rhs) const
            { return lowId_==rhs.lowId_ && highId_==rhs.highId_ && tm_==rhs.tm_; }
    private:
        VertexId lowId_;
        VertexId highId_;
        Timestamp tm_;
    };

} } /* namespace */

namespace std { 
    template<>
    struct hash<intergdb::core::EdgeTriplet> {
      inline size_t operator()(intergdb::core::EdgeTriplet const & et) const {
          size_t hval = 17;
          hval = hval * 37 + hash<intergdb::core::VertexId>()(et.getLowId());
          hval = hval * 37 + hash<intergdb::core::VertexId>()(et.getHighId());
          hval = hval * 37 + hash<intergdb::core::Timestamp>()(et.getTime());
          return hval;
      }
    };
} /* namespace */

namespace intergdb { namespace core
{
    inline std::ostream & operator << (std::ostream & out, Block const & block)
    {
        out << "Block id: " << block.id() << "\n";
        for (auto it=block.getNeighborLists().begin(),
                 eit=block.getNeighborLists().end(); it!=eit; ++it) {
            VertexId headVertex = it->first;
            auto const & nlist = it->second;
            assert(nlist.headVertex()==headVertex);
            out << "Head vertex: " << headVertex << ", edges: ";
            for (auto it = nlist.getEdges().begin(), eit=nlist.getEdges().end();
                    it != eit; ++it)
                out << "(" << it->getToVertex() << ", " << it->getTime() << "),";
            out << "\n";
        }
        return out;
    }

    // TODO: variable length encode timestamps after applying a delta
    // TODO: optional compression
    inline NetworkByteBuffer & operator << (NetworkByteBuffer & sbuf, Block const & block)
    {
        std::unordered_set<EdgeTriplet> written;
        sbuf << block.id();
        auto const & neigs = block.getNeighborLists();
        for (auto it=neigs.begin(); it!=neigs.end(); ++it)  {
            VertexId headVertex = it->first;
            sbuf << headVertex;
            auto const & nlist = it->second;
            auto const & edges = nlist.getEdges();
            sbuf << (uint32_t) (edges.size());
            for (auto nit=edges.begin(); nit!=edges.end(); ++nit) {
                auto const & edge = *nit;
                sbuf << edge.getToVertex();
                sbuf << edge.getTime();
                EdgeTriplet etrip(headVertex, edge.getToVertex(), edge.getTime());
                if (written.count(etrip)==0) {
                    sbuf << *edge.getData();
                    written.insert(etrip);
                }
            }
        }
        return sbuf;
    }

    inline NetworkByteBuffer & operator >> (NetworkByteBuffer & sbuf, Block & block)
    {
        std::unordered_map<EdgeTriplet, std::shared_ptr<EdgeData> > read;
        sbuf >> block.id();
        while (sbuf.getNRemainingBytes()>0) {
            VertexId headVertex;
            sbuf >> headVertex;
            uint32_t nedges;
            sbuf >> nedges;
            for (size_t i=0; i<nedges; ++i) {
                VertexId toVertex;
                sbuf >> toVertex;
                Timestamp tm;
                sbuf >> tm;
                std::shared_ptr<EdgeData> sdata;
                EdgeTriplet etrip(headVertex, toVertex, tm);
                auto it = read.find(etrip);
                if (it==read.end()) {
                    sdata.reset(new EdgeData());
                    sbuf >> *sdata;
                    read[etrip] = sdata;
                } else {
                    sdata = it->second;
                }
                block.addEdge(headVertex, toVertex, tm, sdata);
            }
        }
        return sbuf;
    }
} } /* namespace */

#endif /* INTERGDB_BLOCK_H */
