#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>
#include <Solver.h>
#include <OptimalCommon.h>

using namespace intergdb::common;

struct var_env;
struct gurobi_ctx;

class OptimalOverlapping : public OptimalCommon
{
public:
    OptimalOverlapping() {}
    ~OptimalOverlapping() {}

private:
    int constraints(var_env *e, gurobi_ctx *ctx, QueryWorkload * workload) ;

};


