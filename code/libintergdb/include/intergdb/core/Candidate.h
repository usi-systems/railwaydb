#ifndef INTERGDB_CANDIDATE_H
#define INTERGDB_CANDIDATE_H

#include <intergdb/core/Conf.h>
#include <intergdb/core/NeighborList.h>
#include <intergdb/core/PriorityQueue.h>

#include <unordered_map>
#include <memory>

namespace intergdb { namespace core
{
    class SUEdge
    {
    private:
        VertexId u_, v_;
    public:
        SUEdge(VertexId u, VertexId v)
            { if (u>v) { u_ = v; v_ = u; } else { u_ = u; v_ = u; } }
        VertexId getFromVertex() const { return v_; }
        VertexId getToVertex() const { return u_; }
        bool operator == (SUEdge const & other) const
            { return u_==other.u_ && v_==other.v_; }
    };
} } /* namespace */

namespace std { 
    using namespace intergdb::core;
    template<>
    struct hash<SUEdge> {
      inline size_t operator()(SUEdge const & e) const
      {
          size_t hval = 17;
          hval = hval * 37 + hash<VertexId>()(e.getFromVertex());
          hval = hval * 37 + hash<VertexId>()(e.getToVertex());
          return hval;
      }
    };
} /* namespace */

namespace intergdb { namespace core
{
    class ExpirationMap;
    class NeighborList;

    class Candidate
    {
    public:
        typedef std::unordered_map<VertexId, size_t> EdgeCounts;
        typedef std::unordered_map<VertexId, size_t> PresentVertices;
        Candidate() : size_(sizeof(BlockId)), edgeCount_(0), outEdgeCount_(0) {}
        double getLocality(Conf::SmartLayoutConf::LocalityMetric metric) const;
        EdgeCounts const & getEdgeCounts() const { return numEdges_; }
        size_t getEdgeCount(VertexId headVertex) const
            { return (numEdges_.count(headVertex)==0) ? 0 : numEdges_.find(headVertex)->second; }
        PresentVertices const & getPresentVertices() const { return presentVertices_; }
        Timestamp getNewestTime() const { return timestamps_.getTopItem().key(); }
        size_t getSerializedSize() const  { return size_; }
        size_t getNumEdges() const { return edgeCount_; }
        size_t getNumDanglingEdges() const { return outEdgeCount_; }
        void setEdgeCount(ExpirationMap const & map, VertexId headVertex, size_t n);
        bool hasReverseEdge(ExpirationMap const & map, VertexId headVertex,
                            NeighborList::Edge const & edge);
        std::ostream & print(std::ostream & out, ExpirationMap const & map);
    private:
        size_t size_;
        size_t edgeCount_; // total edge count
        size_t outEdgeCount_; // num of edges going out
        std::unordered_map<VertexId, size_t> presentVertices_; // vertices that appear in neiglists
        PriorityQueue<Timestamp, VertexId, PriorityQueueOrder::MaxAtTop> timestamps_; // max timestamps of neiglists
        std::unordered_map<VertexId, size_t> numEdges_; // number of edges from each neiglists
        std::unordered_map<SUEdge, size_t> internalEdges_; // neiglist to neiglist edges
    };
} } /* namespace */

#endif /* INTERGDB_CANDIDATE_H */
