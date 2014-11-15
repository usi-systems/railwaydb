#pragma once

#include <intergdb/core/Conf.h>
#include <intergdb/core/NeighborList.h>
#include <intergdb/core/PriorityQueue.h>

#include <unordered_map>

namespace intergdb { namespace core
{
    class AttributeData;
    class Block;
    class Candidate;
    class Edge;
    class HistoricalGraph;
    class UEdge;

    class BlockStats {
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
        void update(Candidate const & candidate);
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
        void addEdge(UEdge const & edge, std::shared_ptr<AttributeData> data);
        void flush();
        std::unordered_map<VertexId, NeighborList > const & getNeighborLists() const
            { return neigLists_; }
        BlockStats const & getBlockStats() const { return stats_; }
    private:
        HeadVertexScorer * getHeadVertexScorer();
        void removeEdges(Block const & block);
        void removeEdge(Edge const & edge);
        void writeBlock();
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
        Schema const & edgeSchema_;
        std::auto_ptr<HeadVertexScorer> hvScorer_;
        PriorityQueue<double, VertexId> scoreQueue_;
        std::unordered_map<VertexId, NeighborList > neigLists_;
    };

} } /* namespace */

