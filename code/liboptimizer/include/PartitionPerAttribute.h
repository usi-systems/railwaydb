#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>

#include <Solver.h>

using namespace intergdb::common;

class PartitionPerAttribute : public Solver
{
public:
    PartitionPerAttribute() { }
    ~PartitionPerAttribute() { }
    Partitioning * solve(QueryWorkload * workload);
};

