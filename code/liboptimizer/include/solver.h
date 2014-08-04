#pragma once

#include <intergdb/common/QueryWorkload.h>


using namespace intergdb::common;


struct var_env;
struct gurobi_ctx;

class Solver
{
public:
    Solver() {}
    int solve(QueryWorkload * workload, int option);

private:
    int x(var_env *e, int a, int p);
    int y(var_env *e, int p, int q);
    int z(var_env *e, int a, int p, int q);
    int u(var_env *e, int p);
    int c_e();
    int c_n();
    int s(std::vector<Attribute> const & attributes);
    double K();
    double alpha();
    int accesses(std::vector<Query> const & queries, int q, int a);   
    void name_variables(var_env *e, char** vname);
    void create_env(var_env *e, QueryWorkload * workload);
    void init_ctx(var_env *e, gurobi_ctx* ctx);
    void variables(var_env *e, gurobi_ctx *ctx);   
    void objective(var_env *e, gurobi_ctx *ctx, QueryWorkload * workload);
    int nov_constraints(var_env *e, gurobi_ctx *ctx, QueryWorkload * workload);
    int ov_constraints(var_env *e, gurobi_ctx *ctx, QueryWorkload * workload);
    int solve_model(var_env *e, gurobi_ctx *ctx);
    void cleanup(var_env *e, gurobi_ctx *ctx);

};


