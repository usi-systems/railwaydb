#include <intergdb/core/Candidate.h>

#include <intergdb/core/ExpirationMap.h>
#include <intergdb/core/Helper.h>

using namespace std;
using namespace intergdb::core;

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

double Candidate::getLocality(Conf::SmartLayoutConf::LocalityMetric metric) const
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