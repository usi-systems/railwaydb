#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>

#include <Solver.h>

using namespace intergdb::common;

class HeuristicOverlapping : public Solver
{
public:
    HeuristicOverlapping() { }
    ~HeuristicOverlapping() { }
    Partitioning * solve(QueryWorkload * workload);
};

