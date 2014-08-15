
#include <assert.h>
#include <iostream>

#include <PartitionPerAttribute.h>

#include <intergdb/common/SystemConstants.h>

using namespace std;
using namespace intergdb::common;
using namespace intergdb::optimizer;

Partitioning PartitionPerAttribute::solve(QueryWorkload const & workload) 
{
    Partition partition;
    Partitioning partitioning;
    for (auto & attribute : workload.getAttributes()) {
        partition.clearAttributes();
        partition.addAttribute(&attribute);
        partitioning.addPartition(partition);
    }  
    return partitioning;
}
   
