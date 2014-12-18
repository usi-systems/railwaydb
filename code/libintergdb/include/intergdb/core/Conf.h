#pragma once

#include <intergdb/common/Attribute.h>
#include <intergdb/common/Types.h>
#include <intergdb/core/Schema.h>


#include <string>
#include <cassert>

using namespace intergdb::common;

namespace intergdb { namespace core
{
    class Conf
    {
    public:
        class SmartLayoutConf
        {
        public:
            enum LocalityMetric { LM_RCONDUCTANCE=0, LM_COHESIVENESS, LM_RCC_MIXED };
            enum InitialCandidateSelectionPolicy { ICS_Random=0, ICS_Old, ICS_New, ICS_Max, ICS_Min, ICS_nPolicies };
            SmartLayoutConf()
                : localityMetric_(LM_RCC_MIXED),
                  initialCandidateSelectionPolicy_(ICS_Old),
                  seedEdgeCount_(1), initialCandidateCount_(10) {}
        public:
            LocalityMetric localityMetric() const
                { return localityMetric_; }
            LocalityMetric & localityMetric()
                { return localityMetric_; }
            InitialCandidateSelectionPolicy initialCandidateSelectionPolicy() const
                { return initialCandidateSelectionPolicy_; }
            InitialCandidateSelectionPolicy & initialCandidateSelectionPolicy()
                { return initialCandidateSelectionPolicy_; }
            size_t initialCandidateCount() const
                { return initialCandidateCount_; }
            size_t & initialCandidateCount()
                { return initialCandidateCount_; }
            size_t seedEdgeCount() const // cannot be changed at this point
                { return seedEdgeCount_; }
            static std::string getInitialCandidateSelectionPolicyName(InitialCandidateSelectionPolicy policy)
            {
                switch(policy) {
                case ICS_Random: return "Random";
                case ICS_Old: return "Old";
                case ICS_New: return "New";
                case ICS_Max: return "Max";
                case ICS_Min: return "Min";
                default: assert(!"cannot happen"); break;
                }
                assert(!"cannot happen");
                return "";
            }
        private:
            LocalityMetric localityMetric_;
            InitialCandidateSelectionPolicy initialCandidateSelectionPolicy_;
            size_t seedEdgeCount_;
            size_t initialCandidateCount_;
        };
        enum LayoutMode { LM_Random=0, LM_Old, LM_Max, LM_Smart };
    public:
    Conf(std::string const & name, std::string const & storageDir, 
         std::vector<std::pair<std::string, Attribute::DataType>> vertexSchema, 
         std::vector<std::pair<std::string, Attribute::DataType>> edgeSchema)
          : name_(name), storageDir_(storageDir),
            windowSize_(1024*1024), // 1M edges
            blockSize_(16*1024), // in bytes (16K)
            blockBufferSize_(16*1024), // in blocks (256MB)
            partitioningBufferSize_(16*1024), // in partitionings 
            vertexDataBufferSize_(100*1024), // 100K vertices
            expirationMapSize_(std::max(windowSize_/100,
                    blockSize_/(sizeof(VertexId)+sizeof(Timestamp)))),
            layoutMode_(LM_Smart),
            vertexSchema_(vertexSchema),
            edgeSchema_(edgeSchema)
        {}

        static std::string getLayoutModeName(LayoutMode layout)
        {
            switch (layout)
            {
            case LM_Random: return "Random";
            case LM_Old: return "Old";
            case LM_Max: return "Max";
            case LM_Smart: return "Smart";
            }
            assert(!"cannot happen");
            return "";
        }

        std::string const & getName() const { return name_; }
        std::string const & getStorageDir() const { return storageDir_; }

        // number of edges to keep in memory
        size_t windowSize() const { return windowSize_; }
        size_t & windowSize() { return windowSize_; }

        // in bytes
        size_t blockSize() const { return blockSize_; }
        size_t & blockSize() { return blockSize_; }

        // in blocks
        size_t blockBufferSize() const { return blockBufferSize_; }
        size_t & blockBufferSize() { return blockBufferSize_; }

        // in partitionings
        size_t partitioningBufferSize() const { return partitioningBufferSize_; }
        size_t & partitioningsBufferSize() { return partitioningBufferSize_; }

        // number of vertices whose data is cached in memory
        size_t vertexDataBufferSize() const { return vertexDataBufferSize_; }
        size_t & vertexDataBufferSize() { return vertexDataBufferSize_; }

        // number of expired edges to keep in memory
        size_t expirationMapSize() const { return expirationMapSize_; }
        size_t & expirationMapSize() { return expirationMapSize_; }

        LayoutMode layoutMode() const { return layoutMode_; }
        LayoutMode & layoutMode() { return layoutMode_; }

        SmartLayoutConf const & smartLayoutConf() const { return layoutConf_; }
        SmartLayoutConf & smartLayoutConf() { return layoutConf_; }

        Schema const & getEdgeSchema() const { return edgeSchema_; }
        Schema const & getVertexSchema() const { return vertexSchema_; }

    private:
        std::string name_;
        std::string storageDir_;
        size_t windowSize_; // number of edges in the in memory graph
        size_t blockSize_; // in bytes
        size_t blockBufferSize_;  // in blocks
        size_t partitioningBufferSize_; // in partitionings
        size_t vertexDataBufferSize_; // number of vertices whose data is cached
        size_t expirationMapSize_; // number of edges in the expiration map
        LayoutMode layoutMode_;
        SmartLayoutConf layoutConf_;
        Schema vertexSchema_;
        Schema edgeSchema_;
    };

} } /* namespace */

