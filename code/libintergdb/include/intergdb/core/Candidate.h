#ifndef INTERGDB_CANDIDATE_H
#define INTERGDB_CANDIDATE_H

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
    //class ExpirationMap;
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

    void Candidate::setEdgeCount(ExpirationMap const & map, VertexId headVertex, size_t n)
    {
        size_t const cn = (numEdges_.count(headVertex)==0) ? 0 : numEdges_[headVertex];
        if (cn==n)
            return; // no change
        auto const & nlist = map.getNeighborLists().find(headVertex)->second;
        if (cn<n) { // we are adding edges
            for (size_t j=cn; j<n; j++) {
                size_ += sizeof(VertexId) + sizeof(Timestamp);
                auto const & edge = nlist.getNthOldestEdge(j);
                VertexId to = edge.getToVertex();
                if (presentVertices_.count(to)==0)
                    presentVertices_[to] = 1;
                else
                    presentVertices_[to]++;
                bool found = hasReverseEdge(map, headVertex, edge);
                if (found) {
                    outEdgeCount_--;
                    SUEdge ie(headVertex, to);
                    if(internalEdges_.count(ie))
                        internalEdges_[ie]++;
                    else
                        internalEdges_[ie] = 1;
                } else {
                    outEdgeCount_++;
                }
                edgeCount_++;
                if (!found)
                    size_ += getSerializedSizeOf(*edge.getData());
            }
        } else { // we are removing edges
            for (ssize_t j=cn-1; j>=(ssize_t)n; j--) {
                size_ -= sizeof(VertexId) + sizeof(Timestamp);
                auto const & edge = nlist.getNthOldestEdge(j);
                VertexId to = edge.getToVertex();
                if (--presentVertices_[to]==0)
                    presentVertices_.erase(to);
                bool found = hasReverseEdge(map, headVertex, edge);
                if (found) {
                    outEdgeCount_++;
                    SUEdge ie(headVertex, to);
                    if(--internalEdges_[ie]==0)
                        internalEdges_.erase(ie);
                } else {
                    outEdgeCount_--;
                }
                 edgeCount_--;
                if (!found)
                    size_ -= getSerializedSizeOf(*edge.getData());
            }
        }
        assert(edgeCount_>=outEdgeCount_);
        if (cn==0) {
            size_ += (sizeof(VertexId) + sizeof(uint32_t));
            presentVertices_[headVertex] = 1;
        }
        if (n==0) {
            numEdges_.erase(headVertex);
            timestamps_.removeItem(headVertex);
            size_ -= (sizeof(VertexId) + sizeof(uint32_t));
            if(--presentVertices_[headVertex]==0)
                presentVertices_.erase(headVertex);
        } else {
            numEdges_[headVertex] = n;
            timestamps_.updateItem(nlist.getNthOldestEdge(n-1).getTime(), headVertex);
        }
    }

    bool Candidate::hasReverseEdge(ExpirationMap const & map, VertexId headVertex,
                                   NeighborList::Edge const & edge)
    {
        bool found = false;
        VertexId toVertex = edge.getToVertex();
        if (numEdges_.count(toVertex)>0) {
            auto const & nlist = map.getNeighborLists().find(toVertex)->second;
            size_t i = numEdges_[toVertex]-1;
            Timestamp startTime = nlist.getOldestEdge().getTime();
            NeighborList::Edge const * oedge = & nlist.getNthOldestEdge(i);
            if (edge.getTime() >= startTime) {
                if (edge.getTime() < oedge->getTime()) {
                    found = true;
                } else {
                    while (!found && edge.getTime()==oedge->getTime()) {
                        if (oedge->getToVertex()==headVertex) {
                            found = true;
                        } else {
                            if (i==0) break;
                            oedge = & nlist.getNthOldestEdge(--i);
                        }
                    }
                }
            }
            //std::shared_ptr dummy;
            //assert (!found || const_cast<NeighborList &>(nlist).getEdgeData(headVertex, edge.getTime(), dummy));
        }
        return found;
    }

    std::ostream & Candidate::print(std::ostream & out, ExpirationMap const & map)
    {
        for (auto it=numEdges_.begin(); it!=numEdges_.end(); ++it) {
            VertexId headVertex = it->first;
            size_t count = it->second;
            out << "Head vertex: " << headVertex << ", edges: ";
            auto const & nlist = map.getNeighborLists().find(headVertex)->second;
            for (size_t i=0; i<count; ++i) {
                auto const & edge = nlist.getNthOldestEdge(i);
                out << "(" << edge.getToVertex() << "," << edge.getTime() << "),";
            }
            out << "\n";
        }
        return out;
    }

    inline double Candidate::getLocality(Conf::SmartLayoutConf::LocalityMetric metric) const
    {
        double rConductance = 1.0 - outEdgeCount_ / ((double)edgeCount_);
        size_t nlistCount = numEdges_.size();
        double cohesiveness = (nlistCount==1) ? 0.5 :
                (internalEdges_.size() / (0.5*nlistCount*(nlistCount-1.0)));
        switch (metric) {
        case Conf::SmartLayoutConf::LM_RCC_MIXED:
            return rConductance * cohesiveness;
        case Conf::SmartLayoutConf::LM_RCONDUCTANCE:
            return rConductance;
        case Conf::SmartLayoutConf::LM_COHESIVENESS:
            return cohesiveness;
        }
    }
} } /* namespace */

#endif /* INTERGDB_CANDIDATE_H */
