#include <intergdb/optimizer/OptimalCommon.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <iostream>
#include <sstream>

#include <intergdb/common/SystemConstants.h>
#include <intergdb/common/Partition.h>

using namespace std;
using namespace intergdb::common;
using namespace intergdb::optimizer;

class GRBEnvironment
{
private:
    GRBEnvironment()
    {
        bool error = GRBloadenv(&env_, NULL);
        if (error) {
            cerr << "ERROR in GRBloadenv: " << GRBgeterrormsg(env_) << endl;
            throw runtime_error(GRBgeterrormsg(env_));
        }
    }
public:
    ~GRBEnvironment()
    {
        GRBfreeenv(env_);
    }
    GRBenv * getNativeHandle()
    {
        return env_;
    }
    static GRBEnvironment & instance()
    {
        static GRBEnvironment genv;
        return genv;
    }
private:
    GRBenv * env_;
};

int OptimalCommon::x(var_env *e, int a, int p)
{
    return (e->x_offset) + (a * e->P + p);
}

int OptimalCommon::y(var_env *e, int p, int q)
{
    return (e->y_offset) + (p * e->Q + q);
}

int OptimalCommon::z(var_env *e, int a, int p, int q)
{
    return (e->z_offset) + ((q * e->A * e->P) + (a * e->P) + p);
}

int OptimalCommon::u(var_env *e, int p)
{
    return (e->u_offset) + p;
}
int OptimalCommon::c_e()
{
    return SystemConstants::numberOfEdgesInABlock;
}

int OptimalCommon::c_n()
{
    return SystemConstants::numberOfNeighborListsInABlock;
}

double OptimalCommon::K()
{
    return 1000;
}

double OptimalCommon::alpha()
{
    return storageThreshold_;
}

double OptimalCommon::s(std::vector<Attribute const *> const & attributes, common::SchemaStats const & stats)
{
    double sum = 0;
    for (auto attribute : attributes) {
        sum += stats.getAvgSize(attribute->getIndex());
    }
    return sum;
}

int OptimalCommon::accesses(std::vector<QuerySummary> const & queries, int q, int a)
{
    for (auto & attribute : queries[q].getAttributes()) {
        if (attribute->getIndex() == a) return 1;
    }
    return 0;
}

void OptimalCommon::name_variables(var_env *e, char** vname)
{
    int j = 0;

    std::stringbuf buffer;
    for (int a = 0; a < e->A; ++a) {
        for (int p = 0; p < e->P; ++p) {
            std::ostringstream oss;
            oss << "x_a" << (a+1) << "_p" << (p+1);
            vname[j] = strdup(oss.str().c_str());
            j++;
        }
    }

    for (int p = 0; p < e->P; ++p) {
        for (int q = 0; q < e->Q; ++q) {
            std::ostringstream oss;
            oss << "y_p" << (p+1) << "_q" << (q+1);
            vname[j] = strdup(oss.str().c_str());
            j++;
        }
    }

    for (int a = 0; a < e->A; ++a) {
        for (int p = 0; p < e->P; ++p) {
            for (int q = 0; q < e->Q; ++q) {
                std::ostringstream oss;
                oss << "z_a" << (a+1) << "_p" << (p+1) << "_q" << (q+1);
                vname[j] = strdup(oss.str().c_str());
                j++;
            }
        }
    }

    for (int p = 0; p < e->P; ++p) {
        std::ostringstream oss;
        oss << "u_p" << (p+1);
        vname[j] = strdup(oss.str().c_str());
        j++;
    }

    assert(e->num_vars == j);
}

void OptimalCommon::print_name_variables(var_env *e, char** vname)
{
    int j = 0;
    cerr << "---- xs ----" << endl;
    for (int a = 0; a < e->A; ++a) {
        for (int p = 0; p < e->P; ++p) {
            cerr << j << ":" << " " << x(e,a,p) << " " << vname[x(e,a,p)] << endl;
            j++;
        }
    }

    cerr << "---- ys ----" << endl;
    for (int p = 0; p < e->P; ++p) {
        for (int q = 0; q < e->Q; ++q) {
            cerr << j << ":" << " " << y(e,p,q) << " " << vname[ y(e,p,q) ] << endl;
            j++;
        }
    }

    cerr << "---- zs ----" << endl;
    for (int a = 0; a < e->A; ++a) {
        for (int p = 0; p < e->P; ++p) {
            for (int q = 0; q < e->Q; ++q) {
                cerr << j << ":" << " " <<  z(e,a,p,q) << " " << vname[z(e,a,p,q)] << endl;
                j++;
            }
        }
    }

    cerr << "---- us ----" << endl;
    for (int p = 0; p < e->P; ++p) {
        cerr << j << ":" << " " << u(e,p) << " " << vname[u(e,p)] << endl;
        j++;
    }

    cerr << e->num_vars << " " << j << endl;
}

void OptimalCommon::create_env(var_env *e, QueryWorkload const * workload)
{
    e->P = workload->getAttributes().size();
    e->Q = workload->getQuerySummaries().size();
    e->A = workload->getAttributes().size();

    e->num_vars
        = (e->A * e->P)         /* xs */
        + (e->P * e->Q)         /* ys */
        + (e->A * e->P * e->Q)  /* zs */
        + (e->P);               /* us */

    e->x_offset = 0;
    e->y_offset = e->A * e->P ;
    e->z_offset = e->y_offset + (e->P * e->Q);
    e->u_offset = e->z_offset + (e->A * e->P * e->Q);
}

void OptimalCommon::init_ctx(var_env *e, gurobi_ctx *ctx)
{
    ctx->obj = new double[e->num_vars];
    ctx->vtype = new char[e->num_vars];
    ctx->ind = new int[e->num_vars];
    ctx->val = new double[e->num_vars];
    ctx->sol = new double[e->num_vars];
    ctx->vname = new char*[e->num_vars];
    ctx->env = GRBEnvironment::instance().getNativeHandle();
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
void OptimalCommon::objective(var_env *e, gurobi_ctx *ctx, QueryWorkload const * workload, common::SchemaStats const & stats)
{

    std::vector<QuerySummary> queries = workload->getQuerySummaries();


    for (int a = 0; a < e->A; ++a) {
        for (int p = 0; p < e->P; ++p) {
            ctx->obj[x(e,a,p)] = 0;
        }
    }

    double val = (SystemConstants::edgeIdSize + SystemConstants::timestampSize) * c_e()
        + (SystemConstants::headVertexSize + SystemConstants::numEntriesSize) * c_n();

    for (int q = 0; q < e->Q; ++q) {

        for (int p = 0; p < e->P; ++p) {
            ctx->obj[y(e,p,q)] = val * workload->getFrequency(queries[q]);
        }

        for (int a = 0; a < e->A; ++a) {
            for (int p = 0; p < e->P; ++p) {
                ctx->obj[z(e,a,p,q)] = stats.getAvgSize(workload->getAttribute(a).getIndex()) * c_e() * workload->getFrequency(queries[q]);
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

    /* Write model to '<classname>.lp' */
    //std::string lpFileName = this->getClassName() + ".lp";
    //error = GRBwrite(ctx->model, lpFileName.c_str());
    //if (error) return error;

    /* Capture solution information */

    error = GRBgetintattr(ctx->model, GRB_INT_ATTR_STATUS, &ctx->optimstatus);
    if (error) return error;

    if (ctx->optimstatus == GRB_OPTIMAL) {
        error = 0;
    } else if (ctx->optimstatus == GRB_INF_OR_UNBD) {
        cerr << "Model is infeasible or unbounded" << endl;
        error = 1;
    } else {
        cerr << "Optimization was stopped early. Status is " << ctx->optimstatus << endl;
        error = 1;
    }
    if (error) return error;

    error = GRBgetdblattr(ctx->model, GRB_DBL_ATTR_OBJVAL, &ctx->objval);
    if (error) return error;

    error = GRBgetdblattrarray(ctx->model, GRB_DBL_ATTR_X, 0, e->num_vars, ctx->sol);
    if (error) return error;

    return error;
}


void OptimalCommon::cleanup(var_env *e, gurobi_ctx *ctx)
{
    if (ctx->vname) {
        for(int i = 0; i < e->num_vars; ++i)
            free(ctx->vname[i]);
        delete [] ctx->vname;
    }
    if (ctx->obj) delete [] ctx->obj;
    if (ctx->vtype) delete [] ctx->vtype;
    if (ctx->ind) delete [] ctx->ind;
    if (ctx->val) delete [] ctx->val;
    if (ctx->sol) delete [] ctx->sol;
    GRBfreemodel(ctx->model);
}

Partitioning OptimalCommon::solve(QueryWorkload const & workload, double storageThreshold, SchemaStats const & stats)
{
    int error = 0;
    gurobi_ctx ctx;
    var_env e;
    storageThreshold_ = storageThreshold;
    create_env(&e, &workload);
    init_ctx(&e, &ctx);

    error = GRBnewmodel(ctx.env, &ctx.model, "storage_optimizer", 0, NULL, NULL, NULL, NULL, NULL);
    if (error) goto QUIT;

    GRBsetintparam(GRBgetenv(ctx.model), GRB_INT_PAR_OUTPUTFLAG, 0);

    variables(&e, &ctx);

    objective(&e, &ctx, &workload, stats);

    /* Add variables */
    error = GRBaddvars(ctx.model, e.num_vars, 0, NULL, NULL, NULL,
                       ctx.obj, NULL, NULL, ctx.vtype,
                       ctx.vname /*NULL if you want default names */);
    if (error) goto QUIT;

    error = GRBsetintattr(ctx.model, GRB_INT_ATTR_MODELSENSE, GRB_MINIMIZE);
    if (error) goto QUIT;

    error = GRBupdatemodel(ctx.model);
    if (error) goto QUIT;

    error = constraints(&e, &ctx, &workload, stats);
    if (error) goto QUIT;

    error = solve_model(&e, &ctx);
    if (error) goto QUIT;

    {
        Partitioning partitioning;
        int j;
        for (int p = 0; p < e.P; ++p) {
            Partition partition;
            for (int a = 0; a < e.A; ++a) {
                j = x(&e,a,p);
                if (ctx.sol[j] == 1)
                    partition.addAttribute(&workload.getAttribute(a));
            }
            if (partition.numAttributes() > 0)
                partitioning.addPartition(partition);
        }
        cleanup(&e, &ctx);
        return partitioning;
    }

QUIT:
    cerr << "ERROR: " << GRBgeterrormsg(ctx.env) << endl;
    cleanup(&e, &ctx);
    throw runtime_error(GRBgeterrormsg(ctx.env));
    return *((Partitioning *)nullptr);
}

