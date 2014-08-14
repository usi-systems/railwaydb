#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>

using namespace intergdb::common;

class Solver
{
public:
    virtual ~Solver() { }
    virtual Partitioning * solve(QueryWorkload * workload) = 0;  
};


