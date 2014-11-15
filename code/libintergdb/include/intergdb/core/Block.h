#pragma once

#include <intergdb/core/Types.h>
#include <intergdb/core/NeighborList.h>
#include <intergdb/core/NetworkByteBuffer.h>
#include <unordered_map>

namespace intergdb { namespace core
{
    class Schema;

    class Block
    {
    public:
        typedef std::unordered_map<VertexId, NeighborList > NeighborListMap;
    public:
        Block();
        Block(Schema const & edgeSchema) : id_(0), serializedSize_(sizeof(BlockId)), edgeSchema_(edgeSchema) {}
        Block(BlockId id, Schema & edgeSchema) : id_ (id), serializedSize_(sizeof(BlockId)), edgeSchema_(edgeSchema) {}
        Block(const Block & other) : id_(other.id_), serializedSize_(other.serializedSize_), edgeSchema_(other.edgeSchema_) {}
        BlockId id() const { return id_; }
        BlockId & id() { return id_; }
        void addEdge(VertexId headVertex, VertexId to, Timestamp tm, std::shared_ptr<AttributeData> data);
        void removeNewestEdge(VertexId headVertex);
        NeighborListMap const & getNeighborLists() const { return neigs_; }
        size_t getSerializedSize() const { return serializedSize_; }
        void swap(Block & other);
        Schema const & getEdgeSchema() { return edgeSchema_; }
        std::ostream & print(std::ostream & out);
    private:
        void addNeighborList(VertexId id);
    private:
        BlockId id_;
        size_t serializedSize_;
        Schema const & edgeSchema_;
        std::unordered_map<VertexId, NeighborList> neigs_;
    };

    std::ostream & operator<<(std::ostream & out, Block const & block);
    NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf, Block const & block);
    NetworkByteBuffer & operator>>(NetworkByteBuffer & sbuf, Block & block);

} } /* namespace */


