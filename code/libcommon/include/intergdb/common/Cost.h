#pragma once

#include <intergdb/common/Partitioning.h>
#include <intergdb/common/QueryWorkload.h>

namespace intergdb { namespace common
{

class Cost
{
public:
    Cost() {}
    double getPartitionSize(Partition const & partition);    
    double getIOCost(Partitioning const & partitioning, QueryWorkload const & workload);
    double getStorageOverhead(Partitioning const & partitioning, QueryWorkload const &workload);

    // the m-function from the paper
    std::vector<Partition const *> getUsedPartitions(std::vector<Partition> const & partitions, 
      std::unordered_set<Attribute const *> const & attributes, Query const & query);
    // used for the non-overlapping heuristic
    double getIOCost(std::vector<Partition> const & partitions, QueryWorkload const & workload, 
      std::unordered_set<Attribute const *> const & filterAttributes);
};

} }
