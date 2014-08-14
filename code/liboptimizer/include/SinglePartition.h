#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>

#include <Solver.h>

using namespace intergdb::common;

class SinglePartition : public Solver
{
public:
    SinglePartition() { }
    ~SinglePartition() { }
    int solve(QueryWorkload * workload, Partitioning * partitioning);
};


