#include <intergdb/core/Block.h>

#include <intergdb/common/Types.h>
#include <intergdb/core/AttributeData.h>
#include <intergdb/core/BlockManager.h>
#include <intergdb/core/Helper.h>
#include <intergdb/core/PartitionIndex.h>
#include <intergdb/core/Schema.h>

#include <algorithm>
#include <cassert>
#include <limits>
#include <unordered_set>
#include <iostream>

using namespace std;
using namespace intergdb::core;
using namespace intergdb::common;

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

void Block::addNeighborList(VertexId id)
{
    if (neigs_.count(id)==0) {
        neigs_[id].headVertex() = id;
        serializedSize_ += sizeof(VertexId);
        serializedSize_ += sizeof(uint32_t);
    }
}

Timestamp Block::getPartitioningTimestamp() const
{
    Timestamp minTimestamp, maxTimestamp;
    findMinMaxTimestamps(minTimestamp, maxTimestamp);
    Timestamp partitioningTimestamp = (minTimestamp+maxTimestamp)/2.0;
    return partitioningTimestamp;
}

void Block::findMinMaxTimestamps(Timestamp & minTimestamp, Timestamp & maxTimestamp) const
{
    minTimestamp = numeric_limits<Timestamp>::max();
    maxTimestamp = 0;
    for (auto it=neigs_.cbegin(); it!=neigs_.cend(); ++it) {
        auto const & nlist = it->second;
        auto const & oldestEdge = nlist.getOldestEdge();
        auto const & newestEdge = nlist.getNewestEdge();
        if (oldestEdge.getTime() < minTimestamp)
            minTimestamp = oldestEdge.getTime();
        if (newestEdge.getTime() > maxTimestamp)
            maxTimestamp = newestEdge.getTime();
    }
}

void Block::addEdge(VertexId headVertex, VertexId to, Timestamp tm,
                              std::shared_ptr<AttributeData> data)
{
    std::shared_ptr<AttributeData> sdata;
    if (neigs_.count(to)>0) {
        auto & nlist = neigs_[to];
        bool there = nlist.getEdgeAttributeData(headVertex, tm, sdata);
        if (!(!there || (sdata.get()==data.get()))) 
            std::cerr << *this << std::endl;
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

void Block::removeNewestEdge(VertexId headVertex)
{
    assert(neigs_.count(headVertex)>0);
    NeighborList & nlist = neigs_[headVertex];
    auto const & edge = nlist.getNewestEdge();
    VertexId to = edge.getToVertex();
    if (neigs_.count(to)>0) {
        auto & nlist = neigs_[to];
        std::shared_ptr<AttributeData> sdata;
        bool there = nlist.getEdgeAttributeData(headVertex, edge.getTime(), sdata);
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


namespace std { 
    template<>
    struct hash<EdgeTriplet> {
        inline size_t operator()(EdgeTriplet const & et) const {
            size_t hval = 17;
            hval = hval * 37 + hash<intergdb::common::VertexId>()(et.getLowId());
            hval = hval * 37 + hash<intergdb::common::VertexId>()(et.getHighId());
            hval = hval * 37 + hash<intergdb::common::Timestamp>()(et.getTime());
            return hval;
        }
    };
}

namespace intergdb { namespace core {


Block Block::recombineBlock(Schema const & schema, BlockManager & bman) const
{
    assert(getSubBlockIds().size()>0);
    Block block;
    block.partition_ = -1;
    std::unordered_map<EdgeTriplet, std::shared_ptr<AttributeData> > read;
    for (auto const & vIdNListPair : getNeighborLists()) {
        VertexId headVertex = vIdNListPair.first;
        auto const & neighborList = vIdNListPair.second;
        int k = 0;
        for (auto const & edge : neighborList.getEdges()) {
            VertexId toVertex = edge.getToVertex();
            Timestamp tm = edge.getTime();
            std::shared_ptr<AttributeData> sdata;
            EdgeTriplet etrip(headVertex, toVertex, tm); 
            auto it = read.find(etrip);
            if (it==read.end()) {
                sdata.reset(schema.newAttributeData());
                for (BlockId id : getSubBlockIds()) {
                    shared_ptr<AttributeData> others = bman.getBlock(id)
                        .getNeighborLists().find(headVertex)->second
                        .getEdges()[k].getData(); 
                    sdata->setAttributes(*others);
                }
                read[etrip] = sdata;
            } else {
                sdata = it->second;
            }
            block.addEdge(headVertex, toVertex, tm, sdata);
            k++;
        }
    }
    return block;
}

vector<Block> Block::partitionBlock(Schema const & schema, PartitionIndex & partitionIndex) const
{
    Timestamp partitioningTimestamp = getPartitioningTimestamp();
    TimeSlicedPartitioning tpart = partitionIndex.getTimeSlicedPartitioning(partitioningTimestamp);
    Partitioning const & part = tpart.getPartitioning();

    vector<Block> blocks;
    blocks.reserve(part.size()+1);
    { // master block (no edge data)
        Block block;
        block.partition_ = -1;
        std::shared_ptr<AttributeData> sdata(schema.newAttributeData(unordered_set<string>()));
        for (auto const & vIdNListPair : getNeighborLists()) {
            VertexId headVertex = vIdNListPair.first;
            auto const & neighborList = vIdNListPair.second;
            for (auto const & edge : neighborList.getEdges()) {
                VertexId toVertex = edge.getToVertex();
                Timestamp tm = edge.getTime();
                EdgeTriplet etrip(headVertex, toVertex, tm); 
                block.addEdge(headVertex, toVertex, tm, sdata);
            }
        }
        blocks.push_back(std::move(block));
    }
    // sub blocks
    for (size_t i=0, iu=part.size(); i<iu; ++i) {
        unordered_set<string> const & attributes = part.at(i);
        Block block;
        block.partition_ = i;
        std::unordered_map<EdgeTriplet, std::shared_ptr<AttributeData> > read;
        for (auto const & vIdNListPair : getNeighborLists()) {
            VertexId headVertex = vIdNListPair.first;
            auto const & neighborList = vIdNListPair.second;
            for (auto const & edge : neighborList.getEdges()) {
                VertexId toVertex = edge.getToVertex();
                Timestamp tm = edge.getTime();
                std::shared_ptr<AttributeData> sdata;
                EdgeTriplet etrip(headVertex, toVertex, tm);
                auto it = read.find(etrip);
                if (it==read.end()) {
                    sdata.reset(schema.newAttributeData(attributes));
                    for (auto const & attrb : attributes) {
                        auto attrbIdx = schema.getIndex(attrb);
                        sdata->setAttribute(attrbIdx, edge.getData()->getAttribute(attrbIdx));
                    }
                    read[etrip] = sdata;
                } else {
                    sdata = it->second;
                }
                block.addEdge(headVertex, toVertex, tm, sdata);
            }
        }
        blocks.push_back(std::move(block));
    }
    return blocks;
}

std::ostream & operator<<(std::ostream & out, Block const & block)
{
    out << "Block id: " << block.id() << "\n";
    for (auto const & headVertexNlistPair : block.getNeighborLists()) {
        VertexId headVertex = headVertexNlistPair.first;
        auto const & nlist = headVertexNlistPair.second;
        assert(nlist.headVertex()==headVertex);
        out << "Head vertex: " << headVertex << ", edges: ";
        for (auto const & edge : nlist.getEdges())
            out << "(" << edge.getToVertex() << ", " << edge.getTime() << "),";
        out << "\n";
    }
    return out;
}

// TODO: variable length encode timestamps after applying a delta
// TODO: optional compression
NetworkByteBuffer & Block::serialize(NetworkByteBuffer & sbuf) const
{
    Timestamp minTimestamp, maxTimestamp;
    findMinMaxTimestamps(minTimestamp, maxTimestamp);
    std::unordered_set<EdgeTriplet> written;
    sbuf << id_;
    sbuf << subBlocks_;
    sbuf << minTimestamp;
    sbuf << maxTimestamp;
    sbuf << partition_;
    auto const & neigs = getNeighborLists();
    for (auto it=neigs.cbegin(); it!=neigs.cend(); ++it)  {
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

NetworkByteBuffer & Block::deserialize(NetworkByteBuffer & sbuf, 
    Schema const & schema, PartitionIndex & partitionIndex) 
{
    Timestamp minTimestamp, maxTimestamp;
    sbuf >> id_;
    sbuf >> subBlocks_;
    sbuf >> minTimestamp;
    sbuf >> maxTimestamp;
    sbuf >> partition_;    
    Timestamp blockTimestamp = (minTimestamp+maxTimestamp)/2.0;
    TimeSlicedPartitioning tpart = partitionIndex.getTimeSlicedPartitioning(blockTimestamp);
    Partitioning const & part = tpart.getPartitioning();
    unordered_set<string> const & attributes = part[partition_];
    std::unordered_map<EdgeTriplet, std::shared_ptr<AttributeData> > read;
    shared_ptr<AttributeData> fsdata(schema.newAttributeData(unordered_set<string>()));
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
            std::shared_ptr<AttributeData> sdata;
            EdgeTriplet etrip(headVertex, toVertex, tm);
            if (partition_ != -1) {
                auto it = read.find(etrip);
                if (it==read.end()) {
                    sdata.reset(schema.newAttributeData(attributes));
                    sbuf >> *sdata;
                    read[etrip] = sdata;
                } else {
                    sdata = it->second;
                }
            } else {
                sdata = fsdata;
            }
            addEdge(headVertex, toVertex, tm, sdata);
        }
    }
    return sbuf;
}

} } 
