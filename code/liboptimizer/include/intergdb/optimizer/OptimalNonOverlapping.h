#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>
#include <intergdb/optimizer/Solver.h>
#include <intergdb/optimizer/OptimalCommon.h>

struct var_env;
struct gurobi_ctx;

namespace intergdb { namespace optimizer {

class OptimalNonOverlapping : public OptimalCommon
{
public:
    OptimalNonOverlapping() {}
    ~OptimalNonOverlapping() {}
    std::string getClassName() { return "OptimalNonOverlapping"; }
private:
    int constraints(var_env *e, gurobi_ctx *ctx, intergdb::common::QueryWorkload const * workload, common::SchemaStats const & stats) ;

};

} }


