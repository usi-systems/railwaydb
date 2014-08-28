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
#include <string>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;
using namespace intergdb;
using namespace intergdb::common;
using namespace intergdb::simulation;
using namespace intergdb::optimizer;

void VsStorageOverheadThreshold::makeQueryIOExp(ExperimentalData * exp) {
  exp->setDescription("Query IO Vs. NumAttributes");
  exp->addField("solver");
  exp->addField("attributes");
  exp->addField("storageOverheadThreshold");
  exp->addField("io");
  exp->setKeepValues(false);
}

void VsStorageOverheadThreshold::makeStorageExp(ExperimentalData * exp) {
  exp->setDescription("Storage Overhead Vs. NumAttributes");
  exp->addField("solver");
  exp->addField("attributes");
  exp->addField("storageOverheadThreshold");
  exp->addField("storage");
  exp->setKeepValues(false);
}

void VsStorageOverheadThreshold::makeRunningTimeExp(ExperimentalData * exp) {
    exp->setDescription("Running Time Vs. NumAttributes");
    exp->addField("solver");
    exp->addField("attributes");
    exp->addField("storageOverheadThreshold");
    exp->addField("time");
    exp->setKeepValues(false);
}

void VsStorageOverheadThreshold::process() 
{
  cerr << "This is an experiment with name: " 
    << this->getClassName() << endl;
  
  SimulationConf simConf;
  Cost cost;
  util::AutoTimer timer;  
  
  ExperimentalData queryIOExp("QueryIOVsStorageOverheadThreshold");
  ExperimentalData runningTimeExp("RunningTimeVsStorageOverheadThreshold");
  ExperimentalData storageExp("StorageOverheadVsStorageOverheadThreshold");

  auto expData = { &queryIOExp, &runningTimeExp, &storageExp };

  makeQueryIOExp(&queryIOExp);
  makeRunningTimeExp(&runningTimeExp);
  makeStorageExp(&storageExp);

  for (auto exp : expData) {
      exp->open();
  }

  int numRuns = 10;
  auto solvers = { SolverFactory::instance().makeSinglePartition(), 
                   SolverFactory::instance().makePartitionPerAttribute(),
                   SolverFactory::instance().makeOptimalOverlapping(), 
                   SolverFactory::instance().makeOptimalNonOverlapping(),
                   SolverFactory::instance().makeHeuristicOverlapping(),
                   SolverFactory::instance().makeHeuristicNonOverlapping() };

  auto storageOverheadThresholds = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0 }; 

  util::RunningStat io;
  util::RunningStat storage;

  double total = solvers.size()  
      * storageOverheadThresholds.size() 
      * numRuns;
  double completed = 0;

  for (int storageOverheadThreshold : storageOverheadThresholds) {
      for (auto solver : solvers) {
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
          runningTimeExp.setFieldValue("storageOverheadThreshold", lexical_cast<std::string>(storageOverheadThreshold));
          runningTimeExp.setFieldValue("time", timer.getRealTimeInSeconds()/numRuns);
              
          queryIOExp.addRecord();
          queryIOExp.setFieldValue("solver", solver->getClassName());
          queryIOExp.setFieldValue("attributes", workload.getAttributes().size());        
          queryIOExp.setFieldValue("storageOverheadThreshold", lexical_cast<std::string>(storageOverheadThreshold));
          queryIOExp.setFieldValue("io", io.getMean());
          io.clear();
              
          storageExp.addRecord();
          storageExp.setFieldValue("solver", solver->getClassName());
          storageExp.setFieldValue("attributes", workload.getAttributes().size());
          storageExp.setFieldValue("storageOverheadThreshold", lexical_cast<std::string>(storageOverheadThreshold));
          storageExp.setFieldValue("storage", storage.getMean());          
          storage.clear();

      }
      cerr << " (" << (completed / total) * 100 << "%)" << endl;
  }
  

  for (auto exp : expData) {
      exp->close();
  }

};
