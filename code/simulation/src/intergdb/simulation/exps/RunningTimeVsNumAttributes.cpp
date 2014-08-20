#include <intergdb/simulation/Experiments.h>
#include <intergdb/simulation/ExperimentalData.h>
#include <intergdb/simulation/SimulationConf.h>

#include <intergdb/util/AutoTimer.h>

#include <intergdb/common/Cost.h>

#include <iostream>
#include <random>
#include <vector>

#include <intergdb/optimizer/Solver.h>
#include <intergdb/optimizer/SolverFactory.h>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;
using namespace intergdb::simulation;
using namespace intergdb::util;
using namespace intergdb::optimizer;

void RunningTimeVsNumAttributes::process() 
{
  cerr << "This is a sample experiment with name: " 
    << this->getClassName() << endl;
  
  SimulationConf simConf;
  AutoTimer timer;
  double storageOverheadThreshold = numeric_limits<double>::max( );

  ExperimentalData exp(getClassName());
  exp.setDescription("Running Time vs. number of attributes.");

  exp.addField("solver");
  exp.addField("attributes");
  exp.addField("time");
  exp.setKeepValues(false);
  exp.open();

  int numRuns = 3;  
  auto solvers = { SolverFactory::instance().makeSinglePartition(), 
                   SolverFactory::instance().makePartitionPerAttribute(),
                   SolverFactory::instance().makeOptimalOverlapping(), 
                   SolverFactory::instance().makeOptimalNonOverlapping() };
  auto attributeCounts = {10, 20, 30, 40, 50};

  for (auto solver : solvers) {
      for (int attributeCount : attributeCounts) {
          simConf.setAttributeCount(attributeCount);
          QueryWorkload workload = simConf.getQueryWorkload();          
          timer.start();
          for (int i = 0; i < numRuns; i++) {                           
              solver->solve(workload, storageOverheadThreshold); 
          }
          timer.stop();
          exp.addRecord();
          exp.setFieldValue("solver", solver->getClassName());
          exp.setFieldValue("attributes", workload.getAttributes().size());
          exp.setFieldValue("time", timer.getRealTimeInSeconds()/numRuns);
      }
  }
  exp.close();
};
