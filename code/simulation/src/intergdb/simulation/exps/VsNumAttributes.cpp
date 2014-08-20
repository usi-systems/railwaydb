#include <intergdb/simulation/Experiments.h>
#include <intergdb/simulation/ExperimentalData.h>
#include <intergdb/simulation/SimulationConf.h>
#include <intergdb/util/RunningStat.h>
#include <intergdb/util/AutoTimer.h>
#include <intergdb/common/Cost.h>
#include <iostream>
#include <random>
#include <vector>
#include <Solver.h>
#include <SolverFactory.h>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;
using namespace intergdb::simulation;
using namespace intergdb::optimizer;

void VsNumAttributes::makeQueryIOExp(ExperimentalData * exp) {
  exp->setDescription("Query IO Vs. NumAttributes");
  exp->addField("solver");
  exp->addField("attributes");
  exp->addField("io");
  exp->setKeepValues(false);
}

void VsNumAttributes::makeStorageExp(ExperimentalData * exp) {
  exp->setDescription("Storage Overhead Vs. NumAttributes");
  exp->addField("solver");
  exp->addField("attributes");
  exp->addField("storage");
  exp->setKeepValues(false);
}

void VsNumAttributes::makeRunningTimeExp(ExperimentalData * exp) {
    exp->setDescription("Running Time Vs. NumAttributes");
    exp->addField("solver");
    exp->addField("attributes");
    exp->addField("time");
    exp->setKeepValues(false);
}

void VsNumAttributes::process() 
{
  cerr << "This is an experiment with name: " 
    << this->getClassName() << endl;
  
  SimulationConf simConf;
  Cost cost;
  util::AutoTimer timer;  
  double storageOverheadThreshold = 1.5;
  
  ExperimentalData queryIOExp("QueryIOVsNumAttributes");
  ExperimentalData runningTimeExp("RunningTimeVsNumAttributes");
  ExperimentalData storageExp("StorageOverheadVsNumAttributes");

  auto expData = { &queryIOExp, &runningTimeExp, &storageExp };

  makeQueryIOExp(&queryIOExp);
  makeRunningTimeExp(&runningTimeExp);
  makeStorageExp(&storageExp);

  for (auto exp : expData) {
      exp->open();
  }

  int numRuns = 3;
  auto solvers = { SolverFactory::instance().makeSinglePartition(), 
                   SolverFactory::instance().makePartitionPerAttribute(),
                   SolverFactory::instance().makeOptimalOverlapping(), 
                   SolverFactory::instance().makeOptimalNonOverlapping(),
                   SolverFactory::instance().makeHeuristicNonOverlapping() };
  auto attributeCounts = {2, 4, 8, 16, 32, 64 }; 
  
  util::RunningStat io;
  util::RunningStat storage;

  double total = solvers.size() * attributeCounts.size()  * numRuns;
  double completed = 0;

  for (auto solver : solvers) {
      for (int attributeCount : attributeCounts) {
          simConf.setAttributeCount(attributeCount);
          QueryWorkload workload = simConf.getQueryWorkload();
          timer.start();
          for (int i = 0; i < numRuns; i++) {
              Partitioning partitioning = solver->solve(workload, storageOverheadThreshold); 
              io.push(cost.getIOCost(partitioning, workload));
              cerr << ".";
          }         
          timer.stop();
          completed += numRuns;

          runningTimeExp.addRecord();
          runningTimeExp.setFieldValue("solver", solver->getClassName());
          runningTimeExp.setFieldValue("attributes", workload.getAttributes().size());
          runningTimeExp.setFieldValue("time", timer.getRealTimeInSeconds()/numRuns);

          queryIOExp.addRecord();
          queryIOExp.setFieldValue("solver", solver->getClassName());
          queryIOExp.setFieldValue("attributes", workload.getAttributes().size());        
          queryIOExp.setFieldValue("io", io.getMean());
          io.clear();

          storageExp.addRecord();
          storageExp.setFieldValue("solver", solver->getClassName());
          storageExp.setFieldValue("attributes", workload.getAttributes().size());
          storageExp.setFieldValue("storage", storage.getMean());          
          storage.clear();

      }
      cerr << " (" << (completed / total) * 100 << "%)" << endl;
  }

  for (auto exp : expData) {
      exp->close();
  }

};
