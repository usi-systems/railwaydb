#pragma once

#include <intergdb/core/AttributeData.h>

#include <memory>
#include <deque>

#include <intergdb/common/Types.h>

using namespace intergdb::common;

namespace intergdb { namespace core
{
    class NeighborList
    {
    public:
        class Edge
        {
        public:
            Edge(VertexId toVertex, Timestamp tm, std::shared_ptr<AttributeData> sdata)
                : toVertex_(toVertex), tm_(tm), data_(sdata) {}
            VertexId getToVertex() const { return toVertex_; }
            Timestamp getTime() const { return tm_; }
            std::shared_ptr<AttributeData> getData() const { return data_; }
            bool operator<(Edge const & rhs) const
                { return tm_<rhs.tm_; }
        private:
            VertexId toVertex_;
            Timestamp tm_;
            std::shared_ptr<AttributeData> data_;
        };
    public:
        VertexId & headVertex() { return headVertex_; }
        VertexId const & headVertex() const { return headVertex_; }
        std::deque<Edge> const & getEdges() const { return edges_; }
        void addEdge(Edge const & edge) { edges_.push_back(edge); }
        Edge const & getNthOldestEdge(size_t n) const { return edges_[n]; }
        Edge const & getOldestEdge() const { return edges_.front(); }
        Edge const & getNewestEdge() const { return edges_.back(); }
        void removeOldestEdge() { if (!edges_.empty()) edges_.pop_front(); }
        void removeNewestEdge() { if (!edges_.empty()) edges_.pop_back(); }
        bool getEdgeAttributeData(VertexId to, Timestamp tm, std::shared_ptr<AttributeData> & sdata);
        bool hasEdgesInRange(Timestamp start, Timestamp end) const;
    private:
        VertexId headVertex_;
        std::deque<Edge> edges_;
    };

} } /* namespace */

