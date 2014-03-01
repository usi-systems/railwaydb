#ifndef INTERGDB_NEIGHBORLIST_H
#define INTERGDB_NEIGHBORLIST_H

#include <tr1/memory>
#include <deque>
#include <tr1/unordered_map>
#include <algorithm>

namespace intergdb { namespace core
{

    template <class EdgeData>
    class NeighborList
    {
    public:
        class Edge
        {
        public:
            Edge(VertexId toVertex, Timestamp tm, std::tr1::shared_ptr<EdgeData> sdata)
                : toVertex_(toVertex), tm_(tm), data_(sdata) {}
            VertexId getToVertex() const { return toVertex_; }
            Timestamp getTime() const { return tm_; }
            std::tr1::shared_ptr<EdgeData> getData() const { return data_; }
            bool operator<(Edge const & rhs) const
                { return tm_<rhs.tm_; }
        private:
            VertexId toVertex_;
            Timestamp tm_;
            std::tr1::shared_ptr<EdgeData> data_;
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
        bool getEdgeData(VertexId to, Timestamp tm, std::tr1::shared_ptr<EdgeData> & sdata);
        bool hasEdgesInRange(Timestamp start, Timestamp end) const;
    private:
        VertexId headVertex_;
        std::deque<Edge> edges_;
    };

    template <class EdgeData>
    bool NeighborList<EdgeData>::getEdgeData(VertexId to, Timestamp tm,
            std::tr1::shared_ptr<EdgeData> & sdata)
    {
        bool found = false;
        Edge tmEdge(to, tm, std::tr1::shared_ptr<EdgeData>());
        auto it = std::lower_bound(edges_.begin(), edges_.end(), tmEdge);
        for (; it!=edges_.end() && it->getTime()==tm; ++it) {
            if (it->getToVertex()==to) {
                sdata = it->getData();
                found = true;
                break;
            }
        }
        assert(!found|| (it->getToVertex()==to && it->getTime()==tm));
        return found;
    }

    template <class EdgeData>
    bool NeighborList<EdgeData>::hasEdgesInRange(Timestamp start, Timestamp end) const
    {
        Edge startEdge(0, start, std::tr1::shared_ptr<EdgeData>());
        auto it = std::lower_bound(edges_.begin(), edges_.end(), startEdge);
        if (it==edges_.end())
            return false;
        if (it->getTime()>=end)
            return false;
        return true;
    }
} } /* namespace */

#endif /* INTERGDB_NEIGHBORLIST_H */
