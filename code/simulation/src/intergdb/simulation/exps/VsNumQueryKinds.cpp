#include <intergdb/simulation/Experiments.h>
#include <intergdb/simulation/ExperimentalData.h>
#include <intergdb/simulation/SimulationConf.h>
#include <intergdb/util/RunningStat.h>
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
using namespace intergdb::optimizer;

void VsNumQueryKinds::makeQueryIOExp(ExperimentalData * exp) {
  exp->setDescription("Query IO Vs. NumAttributes");
  exp->addField("solver");
  exp->addField("attributes");
  exp->addField("queryTypeCount");
  exp->addField("io");
  exp->setKeepValues(false);
}

void VsNumQueryKinds::makeStorageExp(ExperimentalData * exp) {
  exp->setDescription("Storage Overhead Vs. NumAttributes");
  exp->addField("solver");
  exp->addField("attributes");
  exp->addField("queryTypeCount");
  exp->addField("storage");
  exp->setKeepValues(false);
}

void VsNumQueryKinds::makeRunningTimeExp(ExperimentalData * exp) {
    exp->setDescription("Running Time Vs. NumAttributes");
    exp->addField("solver");
    exp->addField("attributes");
    exp->addField("queryTypeCount");
    exp->addField("time");
    exp->setKeepValues(false);
}

void VsNumQueryKinds::process() 
{
  cerr << "This is an experiment with name: " 
    << this->getClassName() << endl;
  
  SimulationConf simConf;
  Cost cost;
  util::AutoTimer timer;  
  double storageOverheadThreshold = 1.5;
  
  ExperimentalData queryIOExp("QueryIOVsNumQueryKinds");
  ExperimentalData runningTimeExp("RunningTimeVsNumQueryKinds");
  ExperimentalData storageExp("StorageOverheadVsNumQueryKinds");

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
  auto attributeCounts = { 8 }; 
  auto queryTypeCounts = {2, 4 }; 

  util::RunningStat io;
  util::RunningStat storage;

  double total = solvers.size()  
      * queryTypeCounts.size() 
      * attributeCounts.size() 
      * numRuns;
  double completed = 0;

  for (auto solver : solvers) {
      for (int attributeCount : attributeCounts) {
          for (int queryTypeCount : queryTypeCounts) {
              simConf.setQueryTypeCount(queryTypeCount);
              
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
              runningTimeExp.setFieldValue("queryTypeCount", simConf.getQueryTypeCount());
              runningTimeExp.setFieldValue("time", timer.getRealTimeInSeconds()/numRuns);
              
              queryIOExp.addRecord();
              queryIOExp.setFieldValue("solver", solver->getClassName());
              queryIOExp.setFieldValue("attributes", workload.getAttributes().size());        
              queryIOExp.setFieldValue("queryTypeCount", simConf.getQueryTypeCount());
              queryIOExp.setFieldValue("io", io.getMean());
              io.clear();
              
              storageExp.addRecord();
              storageExp.setFieldValue("solver", solver->getClassName());
              storageExp.setFieldValue("attributes", workload.getAttributes().size());
              storageExp.setFieldValue("queryTypeCount", simConf.getQueryTypeCount());
              storageExp.setFieldValue("storage", storage.getMean());          
              storage.clear();

          }
          cerr << " (" << (completed / total) * 100 << "%)" << endl;
      }
  }

  for (auto exp : expData) {
      exp->close();
  }

};
