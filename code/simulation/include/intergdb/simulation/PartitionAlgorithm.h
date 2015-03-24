#pragma once

#include <intergdb/simulation/Partition.h>

namespace intergdb { namespace simulation
{

    class PartitionAlgorithm
    {
    public:
        std::vector<Partition> createPartitions(
            QueryWorkload & workload, double storageOverheadLimit) = 0;
    };
} }
