
#include <assert.h>
#include <iostream>

#include <SinglePartition.h>

#include <intergdb/common/SystemConstants.h>

using namespace std;

Partitioning * SinglePartition::solve(QueryWorkload * workload) 
{
    Partition * partition = new Partition();
    for (auto & attribute : workload->getAttributes()) {
        partition->addAttribute(&attribute);
    }
    Partitioning * partitioning = new Partitioning();
    partitioning->addPartition(partition);
    return partitioning;
}