#include <intergdb/simulation/Experiments.h>
#include <intergdb/simulation/ExperimentalData.h>
#include <intergdb/simulation/SimulationConf.h>
#include <intergdb/simulation/Constants.h>
#include <intergdb/util/RunningStat.h>
#include <intergdb/util/AutoTimer.h>
#include <intergdb/common/Cost.h>
#include <intergdb/common/SchemaStats.h>
#include <iostream>
#include <random>
#include <vector>
#include <intergdb/optimizer/Solver.h>
#include <intergdb/optimizer/SolverFactory.h>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;
using namespace intergdb;
using namespace intergdb::common;
using namespace intergdb::simulation;
using namespace intergdb::optimizer;

void VsQueryLength::makeQueryIOExp(ExperimentalData * exp) {
  exp->setDescription("Query IO Vs. QueryLength");
  exp->addField("solver");
  exp->addField("queryLength");
  exp->addField("io");
  exp->addField("deviation");
  exp->setKeepValues(false);
}

void VsQueryLength::makeStorageExp(ExperimentalData * exp) {
  exp->setDescription("Storage Overhead Vs. QueryLength");
  exp->addField("solver");
  exp->addField("queryLength");
  exp->addField("storage");
  exp->addField("deviation");
  exp->setKeepValues(false);
}

void VsQueryLength::makeRunningTimeExp(ExperimentalData * exp) {
    exp->setDescription("Running Time Vs. QueryLength");
    exp->addField("solver");
    exp->addField("queryLength");
    exp->addField("time");
    exp->addField("deviation");
    exp->setKeepValues(false);
}

void VsQueryLength::process() 
{
  cerr << "This is an experiment with name: " 
    << this->getClassName() << endl;
  
  SimulationConf simConf;
  SchemaStats stats;
  Cost cost(stats);
  util::AutoTimer timer;  
  
  ExperimentalData queryIOExp("QueryIOVsQueryLength");
  ExperimentalData runningTimeExp("RunningTimeVsQueryLength");
  ExperimentalData storageExp("StorageOverheadVsQueryLength");

  auto expData = { &queryIOExp, &runningTimeExp, &storageExp };

  makeQueryIOExp(&queryIOExp);
  makeRunningTimeExp(&runningTimeExp);
  makeStorageExp(&storageExp);

  for (auto exp : expData) {
      exp->open();
  }

  auto solvers = { SolverFactory::instance().makeSinglePartition(), 
                   SolverFactory::instance().makePartitionPerAttribute(),
                   SolverFactory::instance().makeOptimalOverlapping(), 
                   SolverFactory::instance().makeOptimalNonOverlapping(),
                   SolverFactory::instance().makeHeuristicOverlapping(),
                   SolverFactory::instance().makeHeuristicNonOverlapping() };

  auto queryLengths = {2, 4, 6, 8, 10, 12, 14, 16 }; 

  double total = solvers.size()  
      * queryLengths.size() 
      * numRuns;
  double completed = 0;

  vector<util::RunningStat> io;
  vector<util::RunningStat> storage;
  vector<util::RunningStat> times;
  vector<std::string> names;

  for (auto solver : solvers) {
      io.push_back(util::RunningStat());
      storage.push_back(util::RunningStat());
      times.push_back(util::RunningStat());
      names.push_back(solver->getClassName());
      vector<std::string> names;  
  }

  int j;
  for (int queryLength : queryLengths) {      
      for (int i = 0; i < numRuns; i++) {
          simConf.setQueryLengthMean(queryLength);              
          QueryWorkload workload = simConf.getQueryWorkload();
          j = 0;
          for (auto solver : solvers) {              
              timer.start();
              Partitioning partitioning = solver->solve(workload, storageOverheadThreshold, stats); 
              timer.stop();                            
              io.at(j).push(cost.getIOCost(partitioning, workload));
              storage.at(j).push(cost.getStorageOverhead(partitioning, workload));   
              times.at(j).push(timer.getRealTimeInSeconds());                    
              j++;
              cerr << ".";
              completed++;
          }
      }

      int j = 0;
      for (auto solver : solvers) {  
   
          runningTimeExp.addRecord();
          runningTimeExp.setFieldValue("solver", solver->getClassName());
          runningTimeExp.setFieldValue("queryLength", lexical_cast<std::string>(queryLength));
          runningTimeExp.setFieldValue("time", times.at(j).getMean());
          runningTimeExp.setFieldValue("deviation", times.at(j).getStandardDeviation());
          times.at(j).clear();
          
          queryIOExp.addRecord();
          queryIOExp.setFieldValue("solver", solver->getClassName());
          queryIOExp.setFieldValue("queryLength", lexical_cast<std::string>(queryLength));
          queryIOExp.setFieldValue("io", io.at(j).getMean());
          queryIOExp.setFieldValue("deviation", io.at(j).getStandardDeviation());
          io.at(j).clear();
          
          storageExp.addRecord();
          storageExp.setFieldValue("solver", solver->getClassName());
          storageExp.setFieldValue("queryLength", lexical_cast<std::string>(queryLength));
          storageExp.setFieldValue("storage", storage.at(j).getMean());    
          storageExp.setFieldValue("deviation", storage.at(j).getStandardDeviation());               
          storage.at(j).clear();

          j++;
      }
      cerr << " (" << (completed / total) * 100 << "%)" << endl;           
  }


  for (auto exp : expData) {
      exp->close();
  }

};
