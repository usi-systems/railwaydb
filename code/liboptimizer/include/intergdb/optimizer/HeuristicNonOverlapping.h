#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>

#include <intergdb/optimizer/Solver.h>

namespace intergdb { namespace optimizer {

class HeuristicNonOverlapping : public Solver
{
public:
    HeuristicNonOverlapping() { }
    ~HeuristicNonOverlapping() { }
    std::string getClassName() { return "HeuristicNonOverlapping"; }
    intergdb::common::Partitioning solve(intergdb::common::QueryWorkload const & workload, double storageThreshold);
    intergdb::common::Partitioning solve(intergdb::common::QueryWorkload const & workload, double storageThreshold, int numPartitions);
};

} }
