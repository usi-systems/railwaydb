#include <intergdb/optimizer/OptimalNonOverlapping.h>

#include <intergdb/common/SystemConstants.h>
#include <intergdb/common/Cost.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <numeric>
#include <iostream>

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

int OptimalNonOverlapping::constraints(var_env *e, gurobi_ctx *ctx, QueryWorkload const * workload, common::SchemaStats const & stats)
{
    int error = 0;
    int j = 0;
    std::vector<QuerySummary> const & queries = workload->getQuerySummaries();

    std::cout << "************ OptimalNonOverlapping::constraints ************" << std::endl;


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
        std::cout << "************ OptimalNonOverlapping::Second set of constraints (A) ************" << std::endl;
        for (int q = 0; q < e->Q; ++q) {
        std::cout << "************ OptimalNonOverlapping::Second set of constraints (B) ************" << std::endl;
            j = 0;
            for (int a = 0; a < e->A; ++a) {
        std::cout << "************ OptimalNonOverlapping::Second set of constraints (C) ************" << std::endl;
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

    /* Third set of constraints */    
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

    /* Fourth set of constraints */    
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
    
    /* Fifth set of constraints */    
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

    /* Sixth set of constraints */    
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

    
    auto attributes = workload->getAttributes();
    Cost cost(stats);
    Partition part; 
    for_each(attributes.begin(), attributes.end(), [&] (Attribute const & attr) {
            part.addAttribute(&attr);            
        });
    
    double limit = 1.0 + alpha() /  (1.0 - (s(workload->getAttributes(), stats) * c_e()) / cost.getPartitionSize(part));

    /* 6th set of constraints */    
    j = 0;
    for (int p = 0; p < e->P; ++p) {
        int index = u(e,p);
        ctx->ind[j] = index;
        ctx->val[j] = 1;
        j++;
    }
    error = GRBaddconstr(ctx->model, e->P, ctx->ind, ctx->val, 
                         GRB_LESS_EQUAL, limit, NULL);
    if (error) return error;

    error = GRBupdatemodel(ctx->model);
    if (error) return error;

    return error;
}
