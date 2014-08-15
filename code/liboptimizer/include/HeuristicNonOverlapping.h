#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>

#include <Solver.h>

namespace intergdb { namespace optimizer {

class HeuristicNonOverlapping : public Solver
{
public:
    HeuristicNonOverlapping() { }
    ~HeuristicNonOverlapping() { }
    intergdb::common::Partitioning * solve(intergdb::common::QueryWorkload * workload);
};

} }