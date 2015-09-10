#pragma once

#include <intergdb/common/Partitioning.h>
#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/SchemaStats.h>

namespace intergdb { namespace common
{
    class Cost
    {
    public:
        Cost(SchemaStats const & stats)
          : stats_(stats)
        {}

        double getPartitionSize(Partition const & partition);

        double getIOCost(Partitioning const & partitioning,
                         QueryWorkload const & workload);

        double getStorageOverhead(Partitioning const & partitioning,
                                  QueryWorkload const &workload);

        // the m-function from the paper
        std::vector<Partition const *> getUsedPartitions(
            std::vector<Partition> const & partitions,
            google::dense_hash_set<Attribute const *> const & attributes,
            QuerySummary const & query);

        // used for the non-overlapping heuristic, only considers some of the attributes (filterAttributes)
        double getIOCost(
            std::vector<Partition> const & partitions,
            QueryWorkload const & workload,
            google::dense_hash_set<Attribute const*> const & filterAttributes);

    private:
        SchemaStats const & stats_;
    };
} }

