#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>


namespace intergdb { namespace optimizer {

class Solver
{
public:
    virtual ~Solver() { }
    // TODO: I think this should take a const & and return a non-pointer
    virtual intergdb::common::Partitioning * solve(intergdb::common::QueryWorkload * workload) = 0;  
};

} }

