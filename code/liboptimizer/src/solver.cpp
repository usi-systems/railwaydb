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

/*
  x_a1_p1 x_a2_p1 x_a1_p2 x_a2_p2 
  y_p1_q1 y_p1_q2 y_p2_q1 y_p2_q2 
  z_a1_p1_q1 z_a1_p1_q2 
  z_a1_p2_q1 z_a1_p2_q2 
  z_a2_p1_q1 z_a2_p1_q2 
  z_a2_p2_q1 z_a2_p2_q2  
  u_p1 u_p2 
*/


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

    cout << "----" << endl;
    cout << "printing xs" << endl;
    for (int a = 0; a < e->A; ++a) {
        for (int p = 0; p < e->P; ++p) {
            cout << vname[x(e,a,p)] << endl;
        }
    }
    cout << "----" << endl;

    cout << "----" << endl;
    cout << "printing ys" << endl;

    for (int p = 0; p < e->P; ++p) {
        for (int q = 0; q < e->Q; ++q) {
            cout << vname[y(e,p,q)] << endl;
        }
    }
    cout << "----" << endl;


    cout << "----" << endl;
    cout << "printing zs" << endl;
    for (int a = 0; a < e->A; ++a) {
        for (int p = 0; p < e->P; ++p) {
            for (int q = 0; q < e->Q; ++q) {
                cout << vname[z(e,a,p,q)] << endl;
                
            }
        }
    }
    cout << "----" << endl;

    cout << "----" << endl;
    cout << "printing us" << endl;
    
    for (int p = 0; p < e->P; ++p) {
        cout << vname[u(e,p)] << endl;
    }

    cout << "----" << endl;

}


int Solver::solve_nov(QueryWorkload * workload) 
{

// #define min(X, Y)  ((X) < (Y) ? (X) : (Y))
//#define X(vars,a,p,A,P,offset) = vars[(offset) + (A * a + p)]
/*
  for (auto & query : queries) {
  std::cout << "query" << std::endl;
  }
*/

    GRBenv   *env   = NULL;
    GRBmodel *model = NULL;
    int       error = 0;
    double    *sol;
    int       *ind = NULL;
    double    *val = NULL;
    double    *obj = NULL;
    char      *vtype = NULL;
    char      **vname = NULL;
    int       optimstatus;
    double    objval;

    var_env e;

    std::vector<Attribute> const & attributes = workload->getAttributes();
    std::vector<Query> const & queries = workload->getQueries();

    e.P = attributes.size();
    e.Q = queries.size();
    e.A = attributes.size();

    e.num_vars 
        = e.P * e.Q        /* xs */
        + e.A * e.Q        /* ys */
        + e.P * e.Q * e.A  /* us */
        + e.A;             /* zs */
    
    e.x_offset = 0;
    e.y_offset = e.P * e.Q ;
    e.z_offset = e.y_offset + e.A * e.Q;
    e.u_offset = e.z_offset + e.P * e.Q * e.A;
    int sa = s(attributes);

    int j = 0;

    obj = new double[e.num_vars];
    vtype = new char[e.num_vars];
    ind = new int[e.num_vars];
    val = new double[e.num_vars];
    sol = new double[e.num_vars];
    vname = new char*[e.num_vars];
    for(int i = 0; i < e.num_vars; ++i) {
        vname[i] = new char[20];
    }
    error = GRBloadenv(&env, "storage_optimizer.log");
    if (error) goto QUIT;

    error = GRBnewmodel(env, &model, "storage_optimizer", 0, NULL, NULL, NULL, NULL, NULL);
    if (error) goto QUIT;


    /* Add variables */
    for (int i = 0; i < e.num_vars; ++i) {
        vtype[i] = GRB_BINARY;
    }

    /* objective coefficients for each of the variables */
    for (int a = 0; a < e.A; ++a) {
        for (int p = 0; p < e.P; ++p) {
            obj[x(&e,a,p)] = 0;
        }
    }
    
    for (int p = 0; p < e.P; ++p) {
        for (int q = 0; q < e.Q; ++q) {
            obj[y(&e,p,q)] = (EDGE_ID_SIZE + TIMESTAMP_SIZE) * c_e() + (HEAD_VERTEX_SIZE + NUM_ENTRIES) * c_n();
        }
    }
    
    for (int a = 0; a < e.A; ++a) {
        for (int p = 0; p < e.P; ++p) {
            for (int q = 0; q < e.Q; ++q) {
                obj[z(&e,a,p,q)] = sa * c_e() ;

            }
        }
    }

    for (int p = 0; p < e.P; ++p) {
        obj[u(&e,p)] = 0;
    }

    /* give variables meaningful names */
    name_variables(&e, vname);

    error = GRBaddvars(model, e.num_vars, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype,
                       vname /*NULL*/);
    if (error) goto QUIT;

    for (int q = 0; q < e.num_vars; ++q) {
        cout << vname[q] << endl;
    }

    error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MINIMIZE);
    if (error) goto QUIT;

    error = GRBupdatemodel(model);
    if (error) goto QUIT;

    /* First set of constraints */    
    for (int a = 0; a < e.A; ++a) {
        j = 0;
        for (int p = 0; p < e.P; ++p) {
            ind[j] = x(&e,a,p);
            val[j] = 1.0;
            j++;
        }
        error = GRBaddconstr(model, e.P, ind, val, GRB_EQUAL, 1.0, NULL);
        if (error) goto QUIT;
    }

    /* Second set of constraints */    
    for (int p = 0; p < e.P; ++p) {
        for (int q = 0; q < e.Q; ++q) {
            j = 0;
            for (int a = 0; a < e.A; ++a) {
                ind[j] = x(&e,a,p);
                val[j] = accesses(queries,q,a);
                j++;
            }
            ind[j] = y(&e,p,q);
            val[j] = -1.0;
            error = GRBaddconstr(model, e.A + 1, ind, val, GRB_GREATER_EQUAL, 0.0, NULL);
            if (error) goto QUIT;
        }
    }

    /* Second set of constraints */    
    for (int p = 0; p < e.P; ++p) {
        for (int q = 0; q < e.Q; ++q) {
            j = 0;
            ind[j] = y(&e,p,q);
            val[j] = K();
            j++;
            for (int a = 0; a < e.A; ++a) {
                ind[j] = x(&e,a,p);
                val[j] = -(accesses(queries,q,a));
                j++;
            }
            error = GRBaddconstr(model, e.A + 1, ind, val, GRB_GREATER_EQUAL, 0.0, NULL);
            if (error) goto QUIT;
        }
    }

    /* Third set of constraints */    
    for (int a = 0; a < e.A; ++a) {
        for (int p = 0; p < e.P; ++p) {
            for (int q = 0; q < e.Q; ++q) {
                ind[0] = z(&e,a,p,q);
                val[0] = 1.0;
                ind[1] = x(&e,a,p);
                val[1] = -1.0;
                ind[2] = y(&e,p,q);
                val[2] = -1.0;
                error = GRBaddconstr(model, 3, ind, val, GRB_GREATER_EQUAL, -1.0, NULL);
                if (error) goto QUIT;                
            }
        }
    }
    
    /* Fourth set of constraints */    
    for (int p = 0; p < e.P; ++p) {
        j = 0;
        for (int a = 0; a < e.A; ++a) {
            ind[j] = x(&e,a,p);
            val[j] = 1.0;
            j++;
        }
        ind[j] = u(&e,p);
        val[j] = -1.0;
        error = GRBaddconstr(model, e.A+1, ind, val, GRB_GREATER_EQUAL, 0.0, NULL);
        if (error) goto QUIT;

    }

    /* Fifth set of constraints */    
    for (int p = 0; p < e.P; ++p) {
        j = 0;
        ind[j] = u(&e,p);
        val[j] = K();
        j++;
        for (int a = 0; a < e.A; ++a) {
            ind[j] = x(&e,a,p);
            val[j] = -1.0;
            j++;
        }       
        error = GRBaddconstr(model, e.A+1, ind, val, GRB_GREATER_EQUAL, 0.0, NULL);
        if (error) goto QUIT;    
    }

    /* Sixth set of constraints */    
    j = 0;
    for (int p = 0; p < e.P; ++p) {
        ind[j] = u(&e,p);
        val[j] = -1.0;
    }
    error = GRBaddconstr(model, e.P, ind, val, GRB_LESS_EQUAL, alpha(), NULL);
    if (error) goto QUIT;

    error = GRBupdatemodel(model);
    if (error) goto QUIT;

    /* Optimize model */
    error = GRBoptimize(model);
    if (error) goto QUIT;

    /* Write model to 'mip1.lp' */

    error = GRBwrite(model, "temp.lp");
    if (error) goto QUIT;

    /* Capture solution information */

    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
    if (error) goto QUIT;

    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
    if (error) goto QUIT;

    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, e.num_vars, sol);
    if (error) goto QUIT;

    printf("\nOptimization complete\n");
    if (optimstatus == GRB_OPTIMAL) {
        cout << "Optimal objective:" << objval << endl;
        for (int a = 0; a < e.A; ++a) {
            for (int p = 0; p < e.P; ++p) {
                j = x(&e,a,p);
                cout << vname[j] << " " << sol[j] << endl;
            }
        }
    } else if (optimstatus == GRB_INF_OR_UNBD) {
        cout << "Model is infeasible or unbounded" << endl;
    } else {
        cout << "Optimization was stopped early" << endl;
    }

QUIT:

    if (vname) {
        for(int i = 0; i < e.num_vars; ++i) {
            delete [] vname[i];
        }
        delete [] vname;
    }
    if (obj) delete [] obj;
    if (vtype) delete [] vtype;
    if (ind) delete [] ind;
    if (val) delete [] val;
    if (sol) delete [] sol; 

    /* Error reporting */

    if (error) {
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        exit(1);
    }

    /* Free model */

    GRBfreemodel(model);

    /* Free environment */

    GRBfreeenv(env);

    return 0;
}

int Solver::solve_ov(QueryWorkload * workload) 
{
    assert(false);
    return 0;
}
