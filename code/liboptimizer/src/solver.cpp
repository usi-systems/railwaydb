/* Copyright 2013, Gurobi Optimization, Inc. */

/* This example formulates and solves the following simple MIP model:

     maximize    x +   y + 2 z
     subject to  x + 2 y + 3 z <= 4
                 x +   y       >= 1
     x, y, z binary
*/

#include <stdlib.h>
#include <stdio.h>

#include <solver.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "gurobi_c.h"
#ifdef __cplusplus
}
#endif

#include <intergdb/common/QueryWorkload.h>

using namespace intergdb::common;


/*

    (1) If an attribute is in a query:
    v_a1_q1 = 1
    v_a2_q1 = 0
    v_a1_q2 = 0
    v_a2_q2 = 1

    (2) If an attribute is in a partition:
    v_a1_p1 = ?
    v_a2_p1 = ?
    v_a1_p2 = ?
    v_a2_p2 = ?

   (3) If an partition is used by a query
    v_p1_q1 = ?
    v_p1_q2 = ?
    v_p2_q1 = ?
    v_p2_q2 = ?

   (4) If an attribute and partition are used by a query
    v_a1_p1_q1 = ?
    v_a1_p1_q2 = ?
    v_a2_p2_q1 = ?
    v_a2_p2_q2 = ?

    (5) add constraint that all attributes must appear in a partition
    v_a1_p1 + v_a1_p2  = 1
    v_a2_p1 + v_a2_p2 = 1


    (6) the number of attributes use by query from a partition determines the value of v_p1_q1 (either 0 or 1)
    (v_a1_q1 * v_a1_p1 + v_a2_q1 * v_a2_p1) - v_p1_q1 >= 0

    To ensure that it is 0 or 1, we are going to add to the objective function, where K is some big value:
   - K * v_p1_q1
   - K * v_p1_q2
   - K * v_p2_q1 
   - K * v_p2_q2

    (7) add constraint to make sure that v_a1_p1 and v_p1_q1 determine  v_a1_p1_q_1
    v_a1_p1_q1 >= (v_a1_p1 + v_p1_q1) -1

    (8)  objective function : minimize the following, where the ?'s are constants

   Here is what we want:
   (? * v_a1_p1_q1 + ? * v_a2_p1_q1 + ?)
  + (? * v_a1_p1_q2 + ? * v_a2_p1_q2 + ?)
  + (? * v_a1_p2_q1 + ? * v_a2_p2_q1 + ?)
  + (? * v_a1_p2_q2 + ? * v_a2_p2_q2 + ?)

  - We need a function for computing the partitions
  - m function: given a list of partitions, and a query, 
    it tells us the list of functions that need to be accessed.

  - First, let's write the two m functions:
    overlapping, and non-overlapping versions

  - For each query, we will make a call to this function, and
    then add terms to the objective

  - For each query, call the m function, returns a list of partititons
  
  - For each list of partitions, compute equation 1 (using the constants)

  - For now, we'll have constants for:
  - c_e(B) and c_n(B) 

  - Do that for each query, sum them up

  - Equation 2 (also left hand side of 5) is the objectibe

  - Two summations: sum over the queries, sum over the partitions, equation 1


  - For each query, we need to find how much I/O it uses, which is the m function

  - For each attribute-partition pair, we have a variable
  - We have to then see if each partition is used by a query, 


  - for the constraints, we have the variables sum up to 1 if non-overlapping, if
    overlapping, they can sum up to the number of partitions

  - constraint 2, equation 3 less than some value alpha
  - |P(B)|
  - For each attribute, find the number of partitions it uses, then
    we take the max of those

  - Solve like this:
  - First, we put no constraints, we solve it, if its too many, we
    then re-formulate the problem with fewer partitions, and re-solve

  - Call the solver in a loop, each time reducing the number of partitions 
    in the formulation

 */


int Solver::solve(QueryWorkload * workload) 
{

  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;
  int       error = 0;
  double    sol[3];
  int       ind[3];
  double    val[3];
  double    obj[3];
  char      vtype[3];
  int       optimstatus;
  double    objval;

  /* Create environment */

  error = GRBloadenv(&env, "mip1.log");
  if (error) goto QUIT;

  /* Create an empty model */

  error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
  if (error) goto QUIT;


  /* Add variables */

  obj[0] = 1; obj[1] = 1; obj[2] = 2;
  vtype[0] = GRB_BINARY; vtype[1] = GRB_BINARY; vtype[2] = GRB_BINARY;
  error = GRBaddvars(model, 3, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype,
                     NULL);
  if (error) goto QUIT;

  /* Change objective sense to maximization */

  error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
  if (error) goto QUIT;

  /* Integrate new variables */

  error = GRBupdatemodel(model);
  if (error) goto QUIT;


  /* First constraint: x + 2 y + 3 z <= 4 */

  ind[0] = 0; ind[1] = 1; ind[2] = 2;
  val[0] = 1; val[1] = 2; val[2] = 3;

  error = GRBaddconstr(model, 3, ind, val, GRB_LESS_EQUAL, 4.0, "c0");
  if (error) goto QUIT;

  /* Second constraint: x + y >= 1 */

  ind[0] = 0; ind[1] = 1;
  val[0] = 1; val[1] = 1;

  error = GRBaddconstr(model, 2, ind, val, GRB_GREATER_EQUAL, 1.0, "c1");
  if (error) goto QUIT;

  /* Optimize model */

  error = GRBoptimize(model);
  if (error) goto QUIT;

  /* Write model to 'mip1.lp' */

  error = GRBwrite(model, "mip1.lp");
  if (error) goto QUIT;

  /* Capture solution information */

  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  if (error) goto QUIT;

  error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
  if (error) goto QUIT;

  error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, 3, sol);
  if (error) goto QUIT;

  printf("\nOptimization complete\n");
  if (optimstatus == GRB_OPTIMAL) {
    printf("Optimal objective: %.4e\n", objval);

    printf("  x=%.0f, y=%.0f, z=%.0f\n", sol[0], sol[1], sol[2]);
  } else if (optimstatus == GRB_INF_OR_UNBD) {
    printf("Model is infeasible or unbounded\n");
  } else {
    printf("Optimization was stopped early\n");
  }

QUIT:

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
