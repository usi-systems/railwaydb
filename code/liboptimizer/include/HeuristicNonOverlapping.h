#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>

#include <Solver.h>

using namespace intergdb::common;

class HeuristicNonOverlapping : public Solver
{
public:
    HeuristicNonOverlapping() { }
    ~HeuristicNonOverlapping() { }
    int solve(QueryWorkload * workload, Partitioning * partitioning);
};


