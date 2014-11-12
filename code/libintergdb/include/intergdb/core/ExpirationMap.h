#ifndef INTERGDB_EXPIRATIONMAP_H
#define INTERGDB_EXPIRATIONMAP_H

#include <intergdb/core/Edge.h>
#include <intergdb/core/Helper.h>
#include <intergdb/core/HistoricalGraph.h>
#include <intergdb/core/NeighborList.h>
#include <intergdb/core/PriorityQueue.h>
#include <intergdb/core/Candidate.h>
#include <intergdb/core/EdgeData.h>
#include <intergdb/core/Schema.h>

#include <vector>
#include <algorithm>
#include <unordered_map>

namespace intergdb { namespace core
{
    class BlockStats
    {
    private:
        double count_;
        double size_;
        double numEdges_;
        double numNLists_;
        double avgNListSize_;
        double locality_;
        Conf::SmartLayoutConf::LocalityMetric locMetric_;
    public:
        BlockStats(Conf const & conf) :
            count_(0), size_(0), numEdges_(0),
            numNLists_(0), avgNListSize_(0), locality_(0),
            locMetric_(conf.smartLayoutConf().localityMetric()) {}
        void update(Candidate const & candidate)
        {
            ++count_;
            size_ += candidate.getSerializedSize();
            numEdges_ += candidate.getNumEdges();
            numNLists_ += candidate.getEdgeCounts().size();
            double avgNListSize = 0.0;
            for (auto it=candidate.getEdgeCounts().begin(),
                    eit=candidate.getEdgeCounts().end(); it!=eit; ++it)
                avgNListSize += it->second;
            avgNListSize /= candidate.getEdgeCounts().size();
            avgNListSize_ += avgNListSize;
            locality_ += candidate.getLocality(locMetric_);
        }
        double getAvgSize() const { return size_ /  count_; }
        double getAvgNumEdges() const { return numEdges_ /  count_; }
        double getAvgNumNLists() const { return numNLists_ /  count_; }
        double getAvgNListSize() const { return avgNListSize_ /  count_; }
        double getAvgLocality() const { return locality_ / count_; }
    };

    class ExpirationMap
    {
    private:
        class HeadVertexScorer
        {
        public:
            HeadVertexScorer(ExpirationMap * map) : parent(map) {}
            virtual double score(VertexId v, size_t i) = 0;
        protected:
            ExpirationMap * parent;
        };
        class HeadVertexMinScorer : public HeadVertexScorer
        {
        public:
            HeadVertexMinScorer(ExpirationMap * map) : HeadVertexScorer(map) {}
            double score(VertexId v, size_t i) { return (this->parent->neigLists_[v].getEdges().size()-i); }
        };
        class HeadVertexMaxScorer : public HeadVertexScorer
        {
        public:
            HeadVertexMaxScorer(ExpirationMap * map) : HeadVertexScorer(map) {}
            double score(VertexId v, size_t i) { return -(double)(this->parent->neigLists_[v].getEdges().size()-i); }
        };
        class HeadVertexOldScorer : public HeadVertexScorer
        {
        public:
            HeadVertexOldScorer(ExpirationMap * map) : HeadVertexScorer(map) {}
            double score(VertexId v, size_t i) { return this->parent->neigLists_[v].getNthOldestEdge(i).getTime(); }
        };
        class HeadVertexNewScorer : public HeadVertexScorer
        {
        public:
            HeadVertexNewScorer(ExpirationMap * map) : HeadVertexScorer(map) {}
            double score(VertexId v, size_t i) { return -(double)(this->parent->neigLists_[v].getNthOldestEdge(i).getTime()); }
        };
        class HeadVertexRandScorer : public HeadVertexScorer
        {
        public:
            HeadVertexRandScorer(ExpirationMap * map) : HeadVertexScorer(map) {}
            double score(VertexId v, size_t i) { return rand(); }
        };
    public:
        ExpirationMap(Conf const & conf, HistoricalGraph * histg);
        void addEdge(UEdge const & edge, std::shared_ptr<EdgeData> data);
        void flush(Schema & schema);
        std::unordered_map<VertexId, NeighborList > const & getNeighborLists() const
            { return neigLists_; }
        BlockStats const & getBlockStats() const { return stats_; }
    private:
        HeadVertexScorer * getHeadVertexScorer();
        void removeEdges(Block const & block);
        void removeEdge(Edge const & edge);
        void writeBlock(Schema & schema);
        void getBlock(Block & block);
        void getBlockSmart(Block & block);
        bool extendCandidate(Candidate & candidate);
    private:
        BlockStats stats_;
        size_t size_;
        size_t maxSize_;
        Conf const & conf_;
        Conf::SmartLayoutConf::LocalityMetric locMetric_;
        HistoricalGraph * histg_;
        std::auto_ptr<HeadVertexScorer> hvScorer_;
        PriorityQueue<double, VertexId> scoreQueue_;
        std::unordered_map<VertexId, NeighborList > neigLists_;
    };

    ExpirationMap::ExpirationMap(Conf const & conf, HistoricalGraph * histg)
        : stats_(conf), size_(0), maxSize_(2*conf.expirationMapSize()), conf_(conf), histg_(histg)
    {
        locMetric_ = conf.smartLayoutConf().localityMetric();
        hvScorer_.reset(getHeadVertexScorer());
    }

    void ExpirationMap::addEdge(UEdge const & edge, std::shared_ptr<EdgeData> data)
    {
        typedef NeighborList::Edge NLEdge;
        {
            NeighborList & nlist = neigLists_[edge.getFirstVertex()];
            nlist.headVertex() = edge.getFirstVertex();
            nlist.addEdge(NLEdge(edge.getSecondVertex(), edge.getTime(), data));
            scoreQueue_.updateItem(hvScorer_->score(nlist.headVertex(), 0), nlist.headVertex());
        }
        {
            NeighborList & nlist = neigLists_[edge.getSecondVertex()];
            nlist.headVertex() = edge.getSecondVertex();
            nlist.addEdge(NLEdge(edge.getFirstVertex(), edge.getTime(), data));
            scoreQueue_.updateItem(hvScorer_->score(nlist.headVertex(), 0), nlist.headVertex());
        }
        size_+=2;
        if (size_>maxSize_)
            writeBlock(data->getSchema());
    }

    ExpirationMap::HeadVertexMaxScorer::HeadVertexScorer * ExpirationMap::getHeadVertexScorer()
    {
        if (conf_.layoutMode()==Conf::LM_Max ||
                (conf_.layoutMode()==Conf::LM_Smart &&
                        conf_.smartLayoutConf().initialCandidateSelectionPolicy()==Conf::SmartLayoutConf::ICS_Max))
            return new HeadVertexMaxScorer(this);
        if ((conf_.layoutMode()==Conf::LM_Smart &&
                conf_.smartLayoutConf().initialCandidateSelectionPolicy()==Conf::SmartLayoutConf::ICS_Min))
            return new HeadVertexMinScorer(this);
        if (conf_.layoutMode()==Conf::LM_Old ||
                (conf_.layoutMode()==Conf::LM_Smart &&
                        conf_.smartLayoutConf().initialCandidateSelectionPolicy()==Conf::SmartLayoutConf::ICS_Old))
            return new HeadVertexOldScorer(this);
        if ((conf_.layoutMode()==Conf::LM_Smart &&
                conf_.smartLayoutConf().initialCandidateSelectionPolicy()==Conf::SmartLayoutConf::ICS_New))
            return new HeadVertexNewScorer(this);
        if (conf_.layoutMode()==Conf::LM_Random ||
                (conf_.layoutMode()==Conf::LM_Smart &&
                        conf_.smartLayoutConf().initialCandidateSelectionPolicy()==Conf::SmartLayoutConf::ICS_Random))
            return new HeadVertexRandScorer(this);
        assert(!"cannot happen");
        return 0;
    }

    void ExpirationMap::flush(Schema & schema)
    {
        while(size_>0)
            writeBlock(schema);
    }

    void ExpirationMap::writeBlock(Schema & schema)
    {
        Block block(schema);
        switch(conf_.layoutMode()) {
        case Conf::LM_Smart:
            getBlockSmart(block);
            break;
        default:
            getBlock(block);
            break;
        }
        removeEdges(block);
        assert(neigLists_.size()==scoreQueue_.getSize());
        histg_->addBlock(block);
    }

    void ExpirationMap::removeEdges(Block const & block)
    {
        auto const & nlists = block.getNeighborLists();
        for(auto it = nlists.begin(); it!=nlists.end(); ++it) {
            NeighborList const & nlist = it->second;
            VertexId from = nlist.headVertex();
            auto const & nedge = nlist.getNewestEdge();
            NeighborList & srcNlist = neigLists_[from];
            bool done = false;
            while (!done) {
                auto const & redge = srcNlist.getOldestEdge();
                done = (redge.getToVertex() == nedge.getToVertex() &&
                        redge.getTime() == nedge.getTime());
                srcNlist.removeOldestEdge();
                size_--;
            }
            if (srcNlist.getEdges().size()==0) {
                neigLists_.erase(from);
                scoreQueue_.removeItem(from);
            } else {
                scoreQueue_.updateItem(hvScorer_->score(from, 0), from);
            }
        }
    }

    void ExpirationMap::getBlock(Block & block)
    {
        std::unordered_map<VertexId, size_t> nTaken;
        size_t maxBlockSize = conf_.blockSize();
        VertexId headVertex;
        while (!scoreQueue_.empty() && block.getSerializedSize()<maxBlockSize) {
            auto const & item = scoreQueue_.getTopItem();
            headVertex = item.id();
            if (nTaken.count(headVertex)==0)
                nTaken[headVertex] = 0;
            size_t i = nTaken[headVertex]++;
            NeighborList const & nlist = neigLists_[headVertex];
            auto const & edge = nlist.getNthOldestEdge(i);
            block.addEdge(headVertex, edge.getToVertex(), edge.getTime(), edge.getData());
            if (i==nlist.getEdges().size()-1)
                scoreQueue_.removeItem(headVertex);
            else
                scoreQueue_.updateItem(hvScorer_->score(headVertex, i+1), headVertex);
        }
        if (block.getSerializedSize()>maxBlockSize) {
            block.removeNewestEdge(headVertex);
            scoreQueue_.updateItem(hvScorer_->score(headVertex, block.getNeighborLists().count(headVertex)
                    ? block.getNeighborLists().find(headVertex)->second.getEdges().size() : 0), headVertex);
        }
        {
            Candidate candidate;
            auto const & neigs = block.getNeighborLists();
            for (auto it=neigs.begin(); it!=neigs.end(); ++it) {
                auto const & headVertex = it->first;
                auto const & nlist = it->second;
                candidate.setEdgeCount(*this, headVertex, nlist.getEdges().size());
            }
            stats_.update(candidate);
        }
    }

} } /* namespace */

namespace intergdb { namespace core
{
    void ExpirationMap::getBlockSmart(Block & block)
    {
        std::unordered_set<std::shared_ptr<Candidate> > candidates;
        { // initialize candidates
            size_t l = conf_.smartLayoutConf().seedEdgeCount();
            size_t k = std::min(neigLists_.size(),
                    conf_.smartLayoutConf().initialCandidateCount());
            auto it = scoreQueue_.getItems().begin();
            for (size_t j=0; j<k; ++j, ++it) {
                size_t headVertex = it->id();
                std::shared_ptr<Candidate> candidate(new Candidate());
                size_t edgeCount = std::min(neigLists_[headVertex].getEdges().size(), l);
                assert(edgeCount<=neigLists_[headVertex].getEdges().size());
                candidate->setEdgeCount(*this, headVertex, edgeCount);
                candidates.insert(candidate);
            }
        }

        std::vector<std::shared_ptr<Candidate> > finalized;
        while (!candidates.empty()) {
            for(auto it=candidates.begin(); it!=candidates.end();) {
                auto cit = it++;
                Candidate & candidate = *(*cit);
                size_t oldSize = candidate.getSerializedSize();
                bool done = extendCandidate(candidate);
                if (done) {
                    finalized.push_back(*cit);
                    candidates.erase(cit);
                } else {
                    assert(candidate.getSerializedSize()>oldSize);
                    (void)oldSize;
                }
            }
        }

        size_t bIndex = 0;
        double bValue = -1.0;
        for (size_t j=0, ju=finalized.size(); j<ju; ++j) {
            double ls = finalized[j]->getLocality(locMetric_);
#ifndef NDEBUG
            if (!(ls>=0.0)) {
                std::cerr << finalized[j]->getLocality(locMetric_) << "\n";
                std::cerr << finalized[j]->getNumEdges() << "\n";
                finalized[j]->print(std::cerr, *this) << "\n";
            }
#endif
            assert(ls>=0.0);
            if (ls>bValue) {
                bValue = ls;
                bIndex = j;
            }
        }

        { // form block
            Candidate const & candidate = *finalized[bIndex];
            stats_.update(candidate);
            auto const & counts = candidate.getEdgeCounts();
            for (auto it=counts.begin(); it!=counts.end(); ++it) {
                VertexId headVertex = it->first;
                size_t count = it->second;
                auto const & nlist = neigLists_.find(headVertex)->second;
                for (size_t i=0; i<count; ++i) {
                    auto const & edge = nlist.getNthOldestEdge(i);
                    block.addEdge(headVertex, edge.getToVertex(), edge.getTime(), edge.getData());
                }
            }
        }
    }

    bool ExpirationMap::extendCandidate(Candidate & candidate)
    {
        bool found = false;
        VertexId bestHeadVertex = 0;
        size_t bestEdgeCount = 0;
        double bestScore = 0;
        double initLocality = candidate.getLocality(locMetric_);
        size_t orgSize = candidate.getSerializedSize();
        Timestamp orgMaxTime = candidate.getNewestTime();
        size_t maxBlockSize = conf_.blockSize();
        // consider all alternatives from the present vertices that increase the number of in edges
        // making a copy here !!!!
        auto vertices = candidate.getPresentVertices();
        for(auto it = vertices.begin(); it != vertices.end(); ++it) {
            VertexId headVertex = it->first;
            auto nlIt = neigLists_.find(headVertex);
            if (nlIt==neigLists_.end())
                continue; // the other side was assigned to some other block
            auto const & nlist = nlIt->second;
            size_t orgEdgeCount = candidate.getEdgeCount(headVertex);
            for (size_t edgeCount=orgEdgeCount+1, maxEdgeCount=nlist.getEdges().size();
                    edgeCount<=maxEdgeCount; edgeCount++) {
                size_t i = edgeCount-1; // the new one being added
                auto const & edge = nlist.getNthOldestEdge(i);
                if (edge.getTime()>orgMaxTime)
                    break;
                assert(edgeCount<=neigLists_[headVertex].getEdges().size());
                candidate.setEdgeCount(*this, headVertex, edgeCount);
                size_t size = candidate.getSerializedSize();
                if (size>maxBlockSize)
                    break;
                double locality = candidate.getLocality(locMetric_);
                double score = (locality-initLocality) / (size-orgSize);
                if (!found || score>bestScore) {
                    found = true;
                    bestScore = score;
                    bestHeadVertex = headVertex;
                    bestEdgeCount = edgeCount;
                }
            }
            assert(orgEdgeCount<=neigLists_[headVertex].getEdges().size());
            candidate.setEdgeCount(*this, headVertex, orgEdgeCount);
        }
        // if none found, consider all one step alternatives from the present vertices
        if (!found) {
            for(auto it = vertices.begin(); it != vertices.end(); ++it) {
                VertexId headVertex = it->first;
                auto nlIt = neigLists_.find(headVertex);
                if (nlIt==neigLists_.end())
                    continue; // the other side was assigned to some other block
                auto const & nlist = nlIt->second;
                size_t orgEdgeCount = candidate.getEdgeCount(headVertex);
                size_t maxEdgeCount = nlist.getEdges().size();
                if (orgEdgeCount==maxEdgeCount)
                    continue;
                //auto const & edge = nlist.getNthOldestEdge(orgEdgeCount);
                size_t edgeCount = orgEdgeCount+1;
                assert(edgeCount<=neigLists_[headVertex].getEdges().size());
                candidate.setEdgeCount(*this, headVertex, edgeCount);
                size_t size = candidate.getSerializedSize();
                if (size<=maxBlockSize) {
                    double locality = candidate.getLocality(locMetric_);
                    double score = (locality-initLocality) / (size-orgSize);
                    if (!found || score>bestScore) {
                        found = true;
                        bestScore = score;
                        bestHeadVertex = headVertex;
                        bestEdgeCount = edgeCount;
                    }
                }
                assert(orgEdgeCount<=neigLists_[headVertex].getEdges().size());
                candidate.setEdgeCount(*this, headVertex, orgEdgeCount);
            }
        }
        // if none found, add the first edge that can be added from any head vertex
        auto const & items = scoreQueue_.getItems();
        for (auto it = items.begin(); !found && it!=items.end(); ++it) {
            VertexId headVertex = it->id();
            auto const & nlist = neigLists_[headVertex];
            size_t orgEdgeCount = candidate.getEdgeCount(headVertex);
            size_t maxEdgeCount = nlist.getEdges().size();
            if (orgEdgeCount==maxEdgeCount)
                continue;
            size_t edgeCount = orgEdgeCount+1;
            assert(edgeCount<=neigLists_[headVertex].getEdges().size());
            candidate.setEdgeCount(*this, headVertex, edgeCount);
            size_t size = candidate.getSerializedSize();
            if (size<=maxBlockSize) {
                found = true;
                double locality = candidate.getLocality(locMetric_);
                double score = (locality-initLocality) / (size-orgSize);
                bestScore = score;
                bestHeadVertex = headVertex;
                bestEdgeCount = edgeCount;
            }
            assert(orgEdgeCount<=neigLists_[headVertex].getEdges().size());
            candidate.setEdgeCount(*this, headVertex, orgEdgeCount);
            if (size>maxBlockSize) {
                if (orgEdgeCount!=0)
                    break; // cannot add any edge
                else if (size-(sizeof(VertexId)+sizeof(uint32_t))>maxBlockSize)
                    break; // cannot add any edge
            }
        }
        if (found) {
            assert(bestEdgeCount<=neigLists_[bestHeadVertex].getEdges().size());
            candidate.setEdgeCount(*this, bestHeadVertex, bestEdgeCount);
        }
        // return true if this candidate cannot be extended anymore
        return !found || (candidate.getSerializedSize()==maxBlockSize);
    }

} } /* namespace */

#endif /* INTERGDB_EXPIRATIONMAP_H */
