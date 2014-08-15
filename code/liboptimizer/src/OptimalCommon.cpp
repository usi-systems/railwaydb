#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <OptimalCommon.h>
#include <iostream>

#include <intergdb/common/SystemConstants.h>
#include <intergdb/common/Partition.h>

using namespace std;
using namespace intergdb::common;
using namespace intergdb::optimizer;

int OptimalCommon::x(var_env *e, int a, int p)
{
    return (e->x_offset) + (a * e->A + p);
}

int OptimalCommon::y(var_env *e, int p, int q)
{
    return (e->y_offset) + (p * e->P + q);
}

int OptimalCommon::z(var_env *e, int a, int p, int q)
{
    return (e->z_offset) + ((a * e->A * e->P) + (p * e->P) + q);
}

int OptimalCommon::u(var_env *e, int p)
{
    return (e->u_offset) + p;
}
int OptimalCommon::c_e()
{
    return 1;
}

int OptimalCommon::c_n()
{
    return 1;
}

double OptimalCommon::K()
{
    return 1000;
}

double OptimalCommon::alpha()
{
    return storageThreshold_;
}

int OptimalCommon::s(std::vector<Attribute> const & attributes)
{
    int sum = 0;
    for (auto & attribute : attributes) {
        sum += attribute.getSize();
    }
    return sum;
}

int OptimalCommon::accesses(std::vector<Query> const & queries, int q, int a)
{
    for (auto & attribute : queries[q].getAttributes()) {
        if (attribute->getIndex() == a) return 1;
    }
    return 0;
}

void OptimalCommon::name_variables(var_env *e, char** vname)
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

void OptimalCommon::create_env(var_env *e, QueryWorkload const * workload) 
{
    e->P = workload->getAttributes().size();
    e->Q = workload->getQueries().size();
    e->A = workload->getAttributes().size();
    
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

void OptimalCommon::init_ctx(var_env *e, gurobi_ctx *ctx) 
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

void OptimalCommon::variables(var_env *e, gurobi_ctx *ctx)
{
    /* set variables types */    
    for (int i = 0; i < e->num_vars; ++i) {
        ctx->vtype[i] = GRB_BINARY;
    }
    
    /* give variables meaningful names */    
    name_variables(e, ctx->vname);

}

/* objective coefficients for each of the variables */
void OptimalCommon::objective(var_env *e, gurobi_ctx *ctx, QueryWorkload const * workload) 
{
    int sa = s(workload->getAttributes());

    for (int a = 0; a < e->A; ++a) {
        for (int p = 0; p < e->P; ++p) {
            ctx->obj[x(e,a,p)] = 0;
        }
    }
    
    for (int p = 0; p < e->P; ++p) {
        for (int q = 0; q < e->Q; ++q) {
            ctx->obj[y(e,p,q)] 
                = (SystemConstants::edgeIdSize + SystemConstants::timestampSize) * c_e() 
                + (SystemConstants::headVertexSize + SystemConstants::numEntries) * c_n();
        }
    }
    
    for (int a = 0; a < e->A; ++a) {
        for (int p = 0; p < e->P; ++p) {
            for (int q = 0; q < e->Q; ++q) {
                ctx->obj[z(e,a,p,q)] = sa * c_e() ;
            }
        }
    }

    for (int p = 0; p < e->P; ++p) {
        ctx->obj[u(e,p)] = 0;
    }

}

int OptimalCommon::solve_model(var_env *e, gurobi_ctx *ctx)
{
    int error = 0;


    /* Optimize model */
    error = GRBoptimize(ctx->model);
    if (error) return error;

    /* Write model to 'temp.lp' */
    //error = GRBwrite(ctx->model, "temp.lp");
    //if (error) return error;

    /* Capture solution information */

    error = GRBgetintattr(ctx->model, GRB_INT_ATTR_STATUS, &ctx->optimstatus);
    if (error) return error;

    error = GRBgetdblattr(ctx->model, GRB_DBL_ATTR_OBJVAL, &ctx->objval);
    if (error) return error;

    error = GRBgetdblattrarray(ctx->model, GRB_DBL_ATTR_X, 0, e->num_vars, ctx->sol);
    if (error) return error;

    printf("\nOptimization complete\n");
    if (ctx->optimstatus == GRB_OPTIMAL) {
        error = 0;
    } else if (ctx->optimstatus == GRB_INF_OR_UNBD) {
        cout << "Model is infeasible or unbounded" << endl;
        error = 1;
    } else {
        cout << "Optimization was stopped early" << endl;
        error = 1;
    }

    return error;
}


void OptimalCommon::cleanup(var_env *e, gurobi_ctx *ctx)
{
    if (ctx->vname) {
        for(int i = 0; i < e->num_vars; ++i) {
            delete [] ctx->vname[i];
        }
        delete [] ctx->vname;
    }
    if (ctx->obj) delete [] ctx->obj;
    if (ctx->vtype) delete [] ctx->vtype;
    if (ctx->ind) delete [] ctx->ind;
    if (ctx->val) delete [] ctx->val;
    if (ctx->sol) delete [] ctx->sol; 
    GRBfreemodel(ctx->model);
    GRBfreeenv(ctx->env);
}


Partitioning OptimalCommon::solve(QueryWorkload const & workload, double storageThreshold)
{
    int error = 0;
    gurobi_ctx ctx;
    var_env e;

    storageThreshold_ = storageThreshold;

    create_env(&e, &workload);

    init_ctx(&e, &ctx);

    //error = GRBloadenv(&ctx.env, "storage_optimizer.log");
    error = GRBloadenv(&ctx.env, NULL);
    if (error) goto QUIT;

    error = GRBnewmodel(ctx.env, &ctx.model, "storage_optimizer", 0, NULL, NULL, NULL, NULL, NULL);
    if (error) goto QUIT;


//#define GRB_INT_PAR_OUTPUTFLAG 0
    GRBsetintparam(GRBgetenv(ctx.model), GRB_INT_PAR_OUTPUTFLAG, 0);

    variables(&e, &ctx);

    objective(&e, &ctx, &workload);

    /* Add variables */
    error = GRBaddvars(ctx.model, e.num_vars, 0, NULL, NULL, NULL, 
                       ctx.obj, NULL, NULL, ctx.vtype,
                       ctx.vname /*NULL if you want default names */);
    if (error) goto QUIT;

    error = GRBsetintattr(ctx.model, GRB_INT_ATTR_MODELSENSE, GRB_MINIMIZE);
    if (error) goto QUIT;

    error = GRBupdatemodel(ctx.model);
    if (error) goto QUIT;

    error = constraints(&e, &ctx, &workload);
    if (error) goto QUIT;

    error = solve_model(&e, &ctx);
    if (error) goto QUIT;

QUIT:
    if (error) {
        printf("ERROR: %s\n", GRBgeterrormsg(ctx.env));
        exit(1);
    }

    cleanup(&e, &ctx);
    Partitioning partitioning;
    int j;
    for (int p = 0; p < e.P; ++p) {
        Partition partition;
        for (int a = 0; a < e.A; ++a) {
            j = x(&e,a,p);
            if (ctx.sol[j] == 1) {
                partition.addAttribute(&workload.getAttribute(a));
            }
        }
        if (partition.numAttributes() > 0) {
            partitioning.addPartition(partition);
        }
    }

    cout << "the number of partitions is: " << partitioning.numPartitions() << endl;

    return partitioning;
}

