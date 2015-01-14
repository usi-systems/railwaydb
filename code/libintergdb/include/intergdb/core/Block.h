#pragma once

#include <intergdb/core/Types.h>
#include <intergdb/core/NeighborList.h>
#include <intergdb/core/NetworkByteBuffer.h>
#include <unordered_map>
#include <unordered_set>

namespace intergdb { namespace core
{
    class Schema;
    class PartitionIndex;
    typedef std::vector<std::unordered_set<std::string>> Partitioning;

    class Block
    {
    public:
        typedef std::unordered_map<VertexId, NeighborList > NeighborListMap;
    public:
        Block() : Block(0) {}
        Block(BlockId id) : id_ (id), serializedSize_(sizeof(BlockId)) {}
        BlockId id() const { return id_; }
        BlockId & id() { return id_; }
        void addEdge(VertexId headVertex, VertexId to, Timestamp tm, std::shared_ptr<AttributeData> data);
        void removeNewestEdge(VertexId headVertex);
        NeighborListMap const & getNeighborLists() const { return neigs_; }
        size_t getSerializedSize() const { return serializedSize_; }
        void findMinMaxTimestamps(Timestamp & minTimestamp, Timestamp & maxTimestamp) const;
        std::vector<BlockId> const & getSubBlockIds() const { return subBlocks_; }
        void addSubBlockId(BlockId id) { subBlocks_.push_back(id); }
        std::vector<Block> partitionBlock(Schema const & schema, PartitionIndex & partitionIndex) const;
        NetworkByteBuffer & serialize(NetworkByteBuffer & sbuf) const;
        NetworkByteBuffer & deserialize(NetworkByteBuffer & sbuf, 
            Schema const & schema, PartitionIndex & partitionIndex);
    private:
        void addNeighborList(VertexId id);
    private:
        BlockId id_;
        int partitionIndex_;
        size_t serializedSize_;
        std::vector<BlockId> subBlocks_;
        std::unordered_map<VertexId, NeighborList> neigs_;
    };

    std::ostream & operator<<(std::ostream & out, Block const & block);

} } /* namespace */


