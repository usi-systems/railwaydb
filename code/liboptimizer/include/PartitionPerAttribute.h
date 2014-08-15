#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>

#include <Solver.h>

namespace intergdb { namespace optimizer {

class PartitionPerAttribute : public Solver
{
public:
    PartitionPerAttribute() { }
    ~PartitionPerAttribute() { }
    intergdb::common::Partitioning solve(intergdb::common::QueryWorkload const & workload, double storageThreshold);
};

} }
