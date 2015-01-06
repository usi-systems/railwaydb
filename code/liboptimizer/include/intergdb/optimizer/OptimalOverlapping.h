#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>
#include <intergdb/optimizer/Solver.h>
#include <intergdb/optimizer/OptimalCommon.h>

struct var_env;
struct gurobi_ctx;

namespace intergdb { namespace optimizer {

class OptimalOverlapping : public OptimalCommon
{
public:
    OptimalOverlapping() {}
    ~OptimalOverlapping() {}
    std::string getClassName() { return "OptimalOverlapping"; }
private:
    int constraints(var_env *e, gurobi_ctx *ctx, intergdb::common::QueryWorkload const * workload, common::SchemaStats const & stats) ;

};

} }
