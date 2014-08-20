#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>

#include <intergdb/optimizer/Solver.h>

namespace intergdb { namespace optimizer {

class SinglePartition : public Solver
{
public:
    SinglePartition() { }
    ~SinglePartition() { } 
    std::string getClassName() { return "SinglePartition"; }
    intergdb::common::Partitioning solve(intergdb::common::QueryWorkload const & workload, double storageThreshold);
};

} }
