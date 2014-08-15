
#include <assert.h>
#include <iostream>

#include <SinglePartition.h>

#include <intergdb/common/SystemConstants.h>

using namespace std;
using namespace intergdb::common;
using namespace intergdb::optimizer;

Partitioning SinglePartition::solve(QueryWorkload const & workload, double storageThreshold) 
{
    Partition partition;
    for (auto & attribute : workload.getAttributes()) 
        partition.addAttribute(&attribute);
    Partitioning partitioning;
    partitioning.addPartition(partition);
    return partitioning;
}
