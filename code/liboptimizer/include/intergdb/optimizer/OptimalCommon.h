#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>
#include <intergdb/optimizer/Solver.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "gurobi_c.h"
#ifdef __cplusplus
}
#endif

namespace intergdb { namespace optimizer {

typedef struct var_env {
    int num_vars;
    int x_offset;
    int y_offset;
    int z_offset;
    int u_offset;
    int A;
    int P;
    int Q;
} var_env;

typedef struct gurobi_ctx {
    GRBenv   *env;
    GRBmodel *model;
    double    *sol;
    int       *ind;
    double    *val;
    double    *obj;
    char      *vtype;
    char      **vname;
    int       optimstatus;
    double    objval;
} gurobi_ctx;

class OptimalCommon : public Solver
{
public:
    OptimalCommon() {}
    virtual ~OptimalCommon() {}
    intergdb::common::Partitioning solve(intergdb::common::QueryWorkload const & workload, double storageThreshold, common::SchemaStats const & stats);

protected:
    virtual int constraints(var_env *e, gurobi_ctx *ctx, intergdb::common::QueryWorkload const * workload, common::SchemaStats const & stats) = 0;

    int x(var_env *e, int a, int p);
    int y(var_env *e, int p, int q);
    int z(var_env *e, int a, int p, int q);
    int u(var_env *e, int p);
    int c_e();
    int c_n();
    double s(
        std::vector<intergdb::common::Attribute const *> const & attributes,
        common::SchemaStats const & stats);
    double K();
    double alpha();
    int accesses(std::vector<intergdb::common::QuerySummary> const & queries, int q, int a);
    void name_variables(var_env *e, char** vname);
    void print_name_variables(var_env *e, char** vname);
    void create_env(var_env *e, intergdb::common::QueryWorkload const * workload);
    void init_ctx(var_env *e, gurobi_ctx* ctx);
    void variables(var_env *e, gurobi_ctx *ctx);
    void objective(var_env *e, gurobi_ctx *ctx, intergdb::common::QueryWorkload const * workload, common::SchemaStats const & stats);
    int solve_model(var_env *e, gurobi_ctx *ctx);
    void cleanup(var_env *e, gurobi_ctx *ctx);
    double storageThreshold_;
};

} }
