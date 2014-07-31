#pragma once

#include <intergdb/common/QueryWorkload.h>

using namespace intergdb::common;

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

class Solver
{
public:
    Solver() {}
    int solve_nov(QueryWorkload * workload);
    int solve_ov(QueryWorkload * workload);

private:
    int x(var_env *e, int a, int p);
    int y(var_env *e, int p, int q);
    int z(var_env *e, int a, int p, int q);
    int u(var_env *e, int p);
    double K();
    double alpha();
    int accesses(std::vector<Query> const & queries, int q, int a);   
    void name_variables(var_env *e, char** vname);


};


