#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>

#include <Solver.h>

namespace intergdb { namespace optimizer {


class HeuristicOverlapping : public Solver
{
public:
    HeuristicOverlapping() { }
    ~HeuristicOverlapping() { }
    std::string getClassName() { return "HeuristicOverlapping"; }
    intergdb::common::Partitioning solve(intergdb::common::QueryWorkload const & workload, double storageThreshold);
};

} }
