#pragma once

#include <intergdb/common/QueryWorkload.h>

using namespace intergdb::common;

class Solver
{
public:
    Solver() {}
    int solve(QueryWorkload * workload);

};


