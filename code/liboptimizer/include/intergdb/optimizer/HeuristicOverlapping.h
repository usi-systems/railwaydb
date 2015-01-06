#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>

#include <intergdb/optimizer/Solver.h>

namespace intergdb { namespace optimizer {


class HeuristicOverlapping : public Solver
{
public:
    HeuristicOverlapping() { }
    ~HeuristicOverlapping() { }
    std::string getClassName() { return "HeuristicOverlapping"; }
    intergdb::common::Partitioning solve(intergdb::common::QueryWorkload const & workload, double storageThreshold, common::SchemaStats const & stats);
};

} }
