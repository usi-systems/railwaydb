
#include <assert.h>
#include <iostream>

#include <PartitionPerAttribute.h>

#include <intergdb/common/SystemConstants.h>

using namespace std;

Partitioning * PartitionPerAttribute::solve(QueryWorkload * workload) 
{
    Partitioning * partitioning = new Partitioning();
    Partition * partition;
    for (auto & attribute : workload->getAttributes()) {
        partition = new Partition();
        partition->addAttribute(&attribute);
        partitioning->addPartition(partition);
    }  
    return partitioning;
}
   