#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <OptimalNonOverlapping.h>
#include <iostream>

#include <intergdb/common/SystemConstants.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "gurobi_c.h"
#ifdef __cplusplus
}
#endif

using namespace std;
using namespace intergdb::common;
using namespace intergdb::optimizer;

int OptimalNonOverlapping::constraints(var_env *e, gurobi_ctx *ctx, QueryWorkload * workload)
{
    int error = 0;
    int j = 0;
    std::vector<Query> const & queries = workload->getQueries();

    /* First set of constraints */    
    for (int a = 0; a < e->A; ++a) {
        j = 0;
        for (int p = 0; p < e->P; ++p) {
            ctx->ind[j] = x(e,a,p);
            ctx->val[j] = 1.0;
            j++;
        }
        error = GRBaddconstr(ctx->model, e->P, ctx->ind, ctx->val, 
                             GRB_EQUAL, 1.0, NULL);
        if (error) return error;
    }

    /* Second set of constraints */    
    for (int p = 0; p < e->P; ++p) {
        for (int q = 0; q < e->Q; ++q) {
            j = 0;
            for (int a = 0; a < e->A; ++a) {
                ctx->ind[j] = x(e,a,p);
                ctx->val[j] = accesses(queries,q,a);
                j++;
            }
            ctx->ind[j] = y(e,p,q);
            ctx->val[j] = -1.0;
            error = GRBaddconstr(ctx->model, e->A + 1, ctx->ind, ctx->val, 
                                 GRB_GREATER_EQUAL, 0.0, NULL);
            if (error) return error;
        }
    }

    /* Second set of constraints */    
    for (int p = 0; p < e->P; ++p) {
        for (int q = 0; q < e->Q; ++q) {
            j = 0;
            ctx->ind[j] = y(e,p,q);
            ctx->val[j] = K();
            j++;
            for (int a = 0; a < e->A; ++a) {
                ctx->ind[j] = x(e,a,p);
                ctx->val[j] = -(accesses(queries,q,a));
                j++;
            }
            error = GRBaddconstr(ctx->model, e->A + 1, ctx->ind, ctx->val, 
                                 GRB_GREATER_EQUAL, 0.0, NULL);
            if (error) return error;
        }
    }

    /* Third set of constraints */    
    for (int a = 0; a < e->A; ++a) {
        for (int p = 0; p < e->P; ++p) {
            for (int q = 0; q < e->Q; ++q) {
                ctx->ind[0] = z(e,a,p,q);
                ctx->val[0] = 1.0;
                ctx->ind[1] = x(e,a,p);
                ctx->val[1] = -1.0;
                ctx->ind[2] = y(e,p,q);
                ctx->val[2] = -1.0;
                error = GRBaddconstr(ctx->model, 3, ctx->ind, ctx->val, 
                                     GRB_GREATER_EQUAL, -1.0, NULL);
                if (error) return error;                
            }
        }
    }
    
    /* Fourth set of constraints */    
    for (int p = 0; p < e->P; ++p) {
        j = 0;
        for (int a = 0; a < e->A; ++a) {
            ctx->ind[j] = x(e,a,p);
            ctx->val[j] = 1.0;
            j++;
        }
        ctx->ind[j] = u(e,p);
        ctx->val[j] = -1.0;
        error = GRBaddconstr(ctx->model, e->A+1, ctx->ind, ctx->val, 
                             GRB_GREATER_EQUAL, 0.0, NULL);
        if (error) return error;

    }

    /* Fifth set of constraints */    
    for (int p = 0; p < e->P; ++p) {
        j = 0;
        ctx->ind[j] = u(e,p);
        ctx->val[j] = K();
        j++;
        for (int a = 0; a < e->A; ++a) {
            ctx->ind[j] = x(e,a,p);
            ctx->val[j] = -1.0;
            j++;
        }       
        error = GRBaddconstr(ctx->model, e->A+1, ctx->ind, ctx->val, 
                             GRB_GREATER_EQUAL, 0.0, NULL);
        if (error) return error;    
    }

    /* Sixth set of constraints */    
    j = 0;
    for (int p = 0; p < e->P; ++p) {
        ctx->ind[j] = u(e,p);
        ctx->val[j] = -1.0;
    }
    error = GRBaddconstr(ctx->model, e->P, ctx->ind, ctx->val, 
                         GRB_LESS_EQUAL, alpha(), NULL);
    if (error) return error;

    error = GRBupdatemodel(ctx->model);
    if (error) return error;

    return error;
}
