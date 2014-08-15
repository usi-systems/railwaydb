#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>

#include <Solver.h>

namespace intergdb { namespace optimizer {

class SinglePartition : public Solver
{
public:
    SinglePartition() { }
    ~SinglePartition() { } 
    intergdb::common::Partitioning * solve(intergdb::common::QueryWorkload * workload);
};

} }