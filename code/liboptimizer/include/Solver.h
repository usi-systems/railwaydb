#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>

using namespace intergdb::common;

class Solver
{
public:
    virtual ~Solver() { }
    virtual int solve(QueryWorkload * workload, Partitioning * partitioning) = 0;  
};


