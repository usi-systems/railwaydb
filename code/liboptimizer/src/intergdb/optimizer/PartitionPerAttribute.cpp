
#include <intergdb/optimizer/PartitionPerAttribute.h>

#include <intergdb/common/SystemConstants.h>

#include <assert.h>
#include <iostream>


using namespace std;
using namespace intergdb::common;
using namespace intergdb::optimizer;

Partitioning PartitionPerAttribute::solve(QueryWorkload const & workload, double storageThreshold, SchemaStats const & stats)
{
    Partition partition;
    Partitioning partitioning;
    for (Attribute const * attribute : workload.getAttributes()) {
        partition.clearAttributes();
        partition.addAttribute(attribute);
        partitioning.addPartition(partition);
    }
    return partitioning;
}

