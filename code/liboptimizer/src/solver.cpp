/* Copyright 2013, Gurobi Optimization, Inc. */

/* This example formulates and solves the following simple MIP model:

   maximize    x +   y + 2 z
   subject to  x + 2 y + 3 z <= 4
   x +   y       >= 1
   x, y, z binary

*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <solver.h>
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif
#include "gurobi_c.h"
#ifdef __cplusplus
}
#endif

using namespace std;

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

int Solver::x(var_env *e, int a, int p)
{
    return (e->x_offset) + (a * e->A + p);
}

int Solver::y(var_env *e, int p, int q)
{
    return (e->y_offset) + (p * e->P + q);
}

int Solver::z(var_env *e, int a, int p, int q)
{
    return (e->z_offset) + ((a * e->A * e->P) + (p * e->P) + q);
}

int Solver::u(var_env *e, int p)
{
    return (e->u_offset) + p;
}
int Solver::c_e()
{
    return 1;
}

int Solver::c_n()
{
    return 1;
}

double Solver::K()
{
    return 1000;
}

double Solver::alpha()
{
    return 3.78;
}

int Solver::s(std::vector<Attribute> const & attributes)
{
    int sum = 0;
    for (auto & attribute : attributes) {
        sum += attribute.getSize();
    }
    return sum;
}

int Solver::accesses(std::vector<Query> const & queries, int q, int a)
{
    for (auto & attribute : queries[q].getAttributes()) {
        if (attribute->getIndex() == a) return 1;
    }
    return 0;
}

void Solver::name_variables(var_env *e, char** vname)
{
    int j = 0;
    
    for (int a = 0; a < e->A; ++a) {
        for (int p = 0; p < e->P; ++p) {
            sprintf(vname[j], "x_a%d_p%d", a+1, p+1);
            j++;
        }
    }

    for (int p = 0; p < e->P; ++p) {
        for (int q = 0; q < e->Q; ++q) {
            sprintf(vname[j], "y_p%d_q%d", p+1, q+1);
            j++;
        }
    }

    for (int a = 0; a < e->A; ++a) {
        for (int p = 0; p < e->P; ++p) {
            for (int q = 0; q < e->Q; ++q) {
                sprintf(vname[j], "z_a%d_p%d_q%d", a+1, p+1, q+1);
                j++;
            }
        }
    }

    for (int p = 0; p < e->P; ++p) {
        sprintf(vname[j], "u_p%d", p+1);
        j++;
    }

}

void Solver::create_env(var_env *e, QueryWorkload * workload) 
{
    std::vector<Attribute> const & attributes = workload->getAttributes();
    std::vector<Query> const & queries = workload->getQueries();
    
    e->P = attributes.size();
    e->Q = queries.size();
    e->A = attributes.size();
    
    e->num_vars 
        = e->P * e->Q         /* xs */
        + e->A * e->Q         /* ys */
        + e->P * e->Q * e->A  /* us */
        + e->A;               /* zs */
    
    e->x_offset = 0;
    e->y_offset = e->P * e->Q ;
    e->z_offset = e->y_offset + e->A * e->Q;
    e->u_offset = e->z_offset + e->P * e->Q * e->A;
}

void Solver::init_ctx(var_env *e, gurobi_ctx *ctx) 
{
    ctx->obj = new double[e->num_vars];
    ctx->vtype = new char[e->num_vars];
    ctx->ind = new int[e->num_vars];
    ctx->val = new double[e->num_vars];
    ctx->sol = new double[e->num_vars];
    ctx->vname = new char*[e->num_vars];
    for(int i = 0; i < e->num_vars; ++i) {
        ctx->vname[i] = new char[20];
    }
}

int Solver::solve_nov(QueryWorkload * workload) 
{
    int       error = 0;
    gurobi_ctx ctx;
    var_env e;

    std::vector<Attribute> const & attributes = workload->getAttributes();
    std::vector<Query> const & queries = workload->getQueries();

    create_env(&e, workload);

    int sa = s(attributes);
    int j = 0;
    init_ctx(&e, &ctx);

    error = GRBloadenv(&ctx.env, "storage_optimizer.log");
    if (error) goto QUIT;

    error = GRBnewmodel(ctx.env, &ctx.model, "storage_optimizer", 0, NULL, NULL, NULL, NULL, NULL);
    if (error) goto QUIT;

    /* Add variables */
    for (int i = 0; i < e.num_vars; ++i) {
        ctx.vtype[i] = GRB_BINARY;
    }

    /* objective coefficients for each of the variables */
    for (int a = 0; a < e.A; ++a) {
        for (int p = 0; p < e.P; ++p) {
            ctx.obj[x(&e,a,p)] = 0;
        }
    }
    
    for (int p = 0; p < e.P; ++p) {
        for (int q = 0; q < e.Q; ++q) {
            ctx.obj[y(&e,p,q)] 
                = (EDGE_ID_SIZE + TIMESTAMP_SIZE) * c_e() 
                + (HEAD_VERTEX_SIZE + NUM_ENTRIES) * c_n();
        }
    }
    
    for (int a = 0; a < e.A; ++a) {
        for (int p = 0; p < e.P; ++p) {
            for (int q = 0; q < e.Q; ++q) {
                ctx.obj[z(&e,a,p,q)] = sa * c_e() ;
            }
        }
    }

    for (int p = 0; p < e.P; ++p) {
        ctx.obj[u(&e,p)] = 0;
    }

    /* give variables meaningful names */
    name_variables(&e, ctx.vname);

    error = GRBaddvars(ctx.model, e.num_vars, 0, NULL, NULL, NULL, 
                       ctx.obj, NULL, NULL, ctx.vtype,
                       ctx.vname /*NULL if you want default names */);
    if (error) goto QUIT;

    for (int q = 0; q < e.num_vars; ++q) {
        cout << ctx.vname[q] << endl;
    }

    error = GRBsetintattr(ctx.model, GRB_INT_ATTR_MODELSENSE, GRB_MINIMIZE);
    if (error) goto QUIT;

    error = GRBupdatemodel(ctx.model);
    if (error) goto QUIT;

    /* First set of constraints */    
    for (int a = 0; a < e.A; ++a) {
        j = 0;
        for (int p = 0; p < e.P; ++p) {
            ctx.ind[j] = x(&e,a,p);
            ctx.val[j] = 1.0;
            j++;
        }
        error = GRBaddconstr(ctx.model, e.P, ctx.ind, ctx.val, 
                             GRB_EQUAL, 1.0, NULL);
        if (error) goto QUIT;
    }

    /* Second set of constraints */    
    for (int p = 0; p < e.P; ++p) {
        for (int q = 0; q < e.Q; ++q) {
            j = 0;
            for (int a = 0; a < e.A; ++a) {
                ctx.ind[j] = x(&e,a,p);
                ctx.val[j] = accesses(queries,q,a);
                j++;
            }
            ctx.ind[j] = y(&e,p,q);
            ctx.val[j] = -1.0;
            error = GRBaddconstr(ctx.model, e.A + 1, ctx.ind, ctx.val, 
                                 GRB_GREATER_EQUAL, 0.0, NULL);
            if (error) goto QUIT;
        }
    }

    /* Second set of constraints */    
    for (int p = 0; p < e.P; ++p) {
        for (int q = 0; q < e.Q; ++q) {
            j = 0;
            ctx.ind[j] = y(&e,p,q);
            ctx.val[j] = K();
            j++;
            for (int a = 0; a < e.A; ++a) {
                ctx.ind[j] = x(&e,a,p);
                ctx.val[j] = -(accesses(queries,q,a));
                j++;
            }
            error = GRBaddconstr(ctx.model, e.A + 1, ctx.ind, ctx.val, 
                                 GRB_GREATER_EQUAL, 0.0, NULL);
            if (error) goto QUIT;
        }
    }

    /* Third set of constraints */    
    for (int a = 0; a < e.A; ++a) {
        for (int p = 0; p < e.P; ++p) {
            for (int q = 0; q < e.Q; ++q) {
                ctx.ind[0] = z(&e,a,p,q);
                ctx.val[0] = 1.0;
                ctx.ind[1] = x(&e,a,p);
                ctx.val[1] = -1.0;
                ctx.ind[2] = y(&e,p,q);
                ctx.val[2] = -1.0;
                error = GRBaddconstr(ctx.model, 3, ctx.ind, ctx.val, 
                                     GRB_GREATER_EQUAL, -1.0, NULL);
                if (error) goto QUIT;                
            }
        }
    }
    
    /* Fourth set of constraints */    
    for (int p = 0; p < e.P; ++p) {
        j = 0;
        for (int a = 0; a < e.A; ++a) {
            ctx.ind[j] = x(&e,a,p);
            ctx.val[j] = 1.0;
            j++;
        }
        ctx.ind[j] = u(&e,p);
        ctx.val[j] = -1.0;
        error = GRBaddconstr(ctx.model, e.A+1, ctx.ind, ctx.val, 
                             GRB_GREATER_EQUAL, 0.0, NULL);
        if (error) goto QUIT;

    }

    /* Fifth set of constraints */    
    for (int p = 0; p < e.P; ++p) {
        j = 0;
        ctx.ind[j] = u(&e,p);
        ctx.val[j] = K();
        j++;
        for (int a = 0; a < e.A; ++a) {
            ctx.ind[j] = x(&e,a,p);
            ctx.val[j] = -1.0;
            j++;
        }       
        error = GRBaddconstr(ctx.model, e.A+1, ctx.ind, ctx.val, 
                             GRB_GREATER_EQUAL, 0.0, NULL);
        if (error) goto QUIT;    
    }

    /* Sixth set of constraints */    
    j = 0;
    for (int p = 0; p < e.P; ++p) {
        ctx.ind[j] = u(&e,p);
        ctx.val[j] = -1.0;
    }
    error = GRBaddconstr(ctx.model, e.P, ctx.ind, ctx.val, 
                         GRB_LESS_EQUAL, alpha(), NULL);
    if (error) goto QUIT;

    error = GRBupdatemodel(ctx.model);
    if (error) goto QUIT;

    /* Optimize model */
    error = GRBoptimize(ctx.model);
    if (error) goto QUIT;

    /* Write model to 'mip1.lp' */

    error = GRBwrite(ctx.model, "temp.lp");
    if (error) goto QUIT;

    /* Capture solution information */

    error = GRBgetintattr(ctx.model, GRB_INT_ATTR_STATUS, &ctx.optimstatus);
    if (error) goto QUIT;

    error = GRBgetdblattr(ctx.model, GRB_DBL_ATTR_OBJVAL, &ctx.objval);
    if (error) goto QUIT;

    error = GRBgetdblattrarray(ctx.model, GRB_DBL_ATTR_X, 0, e.num_vars, ctx.sol);
    if (error) goto QUIT;

    printf("\nOptimization complete\n");
    if (ctx.optimstatus == GRB_OPTIMAL) {
        cout << "Optimal objective:" << ctx.objval << endl;
        for (int a = 0; a < e.A; ++a) {
            for (int p = 0; p < e.P; ++p) {
                j = x(&e,a,p);
                cout << ctx.vname[j] << " " << ctx.sol[j] << endl;
            }
        }
    } else if (ctx.optimstatus == GRB_INF_OR_UNBD) {
        cout << "Model is infeasible or unbounded" << endl;
    } else {
        cout << "Optimization was stopped early" << endl;
    }

QUIT:
    if (error) {
        printf("ERROR: %s\n", GRBgeterrormsg(ctx.env));
        exit(1);
    }

    if (ctx.vname) {
        for(int i = 0; i < e.num_vars; ++i) {
            delete [] ctx.vname[i];
        }
        delete [] ctx.vname;
    }
    if (ctx.obj) delete [] ctx.obj;
    if (ctx.vtype) delete [] ctx.vtype;
    if (ctx.ind) delete [] ctx.ind;
    if (ctx.val) delete [] ctx.val;
    if (ctx.sol) delete [] ctx.sol; 
    GRBfreemodel(ctx.model);
    GRBfreeenv(ctx.env);
    return 0;
}

int Solver::solve_ov(QueryWorkload * workload) 
{
    assert(false);
    return 0;
}
