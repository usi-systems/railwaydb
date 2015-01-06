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
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>


using namespace boost;
using namespace std;
using namespace intergdb;
using namespace intergdb::common;
using namespace intergdb::simulation;
using namespace intergdb::optimizer;


void VsStorageOverheadThreshold::makeQueryIOExp(ExperimentalData * exp) {
  exp->setDescription("Query IO Vs. StorageOverheadThreshold");
  exp->addField("solver");
  exp->addField("storageOverheadThreshold");
  exp->addField("io");
  exp->addField("deviation");
  exp->setKeepValues(false);
}

void VsStorageOverheadThreshold::makeStorageExp(ExperimentalData * exp) {
  exp->setDescription("Storage Overhead Vs. StorageOverheadThreshold");
  exp->addField("solver");
  exp->addField("storageOverheadThreshold");
  exp->addField("storage");
  exp->addField("deviation");
  exp->setKeepValues(false);
}

void VsStorageOverheadThreshold::makeRunningTimeExp(ExperimentalData * exp) {
    exp->setDescription("Running Time Vs. StorageOverheadThreshold");
    exp->addField("solver");
    exp->addField("storageOverheadThreshold");
    exp->addField("time");
    exp->addField("deviation");
    exp->setKeepValues(false);
}

void VsStorageOverheadThreshold::process() 
{
  cerr << "This is an experiment with name: " 
    << this->getClassName() << endl;
  
  SimulationConf simConf;
  SchemaStats stats;
  Cost cost(stats);
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

  auto solvers = {       
      SolverFactory::instance().makeSinglePartition(), 
      SolverFactory::instance().makePartitionPerAttribute(),
      SolverFactory::instance().makeOptimalOverlapping(),
      SolverFactory::instance().makeOptimalNonOverlapping(), 
      SolverFactory::instance().makeHeuristicOverlapping(),  
      SolverFactory::instance().makeHeuristicNonOverlapping()
  };
  
  vector<double> storageOverheadThresholds = { 0.0, 0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0 }; 
  
  double total = solvers.size()  
      * storageOverheadThresholds.size() 
      * numRuns;
  double completed = 0;

  vector<util::RunningStat> io;
  vector<util::RunningStat> storage;
  vector<util::RunningStat> times;
  vector<std::string> names;
  vector<bool> errorFlags;


  for (auto solver : solvers) {
      io.push_back(util::RunningStat());
      storage.push_back(util::RunningStat());
      times.push_back(util::RunningStat());
      names.push_back(solver->getClassName());
      vector<std::string> names;  
      errorFlags.push_back(false);
  }

  int j;
  for (double sot : storageOverheadThresholds) {
      for (int i = 0; i < numRuns; i++) {
          QueryWorkload workload = simConf.getQueryWorkload();
          j = 0;
          for (auto solver : solvers) {              
              timer.start();
              Partitioning partitioning;
              try {
                  partitioning = solver->solve(workload, sot, stats); 
              } catch (const runtime_error& error) {
                  cerr << "Unable to find a solution" << endl;
                  errorFlags.at(j) = true;
              }
              timer.stop();
              if (!errorFlags.at(j)) {
                  io.at(j).push(cost.getIOCost(partitioning, workload));
                  storage.at(j).push(cost.getStorageOverhead(partitioning, workload));   
                  times.at(j).push(timer.getRealTimeInSeconds());                    
              }
              j++;
              cerr << ".";
              completed++;
          }
      }

      int j = 0;
      for (auto solver : solvers) {  
          if (!errorFlags.at(j)) {
              runningTimeExp.addRecord();
              runningTimeExp.setFieldValue("solver", solver->getClassName());
              runningTimeExp.setFieldValue("storageOverheadThreshold", boost::str(boost::format("%.2f") % sot));
              runningTimeExp.setFieldValue("time", times.at(j).getMean());
              runningTimeExp.setFieldValue("deviation", times.at(j).getStandardDeviation());
              times.at(j).clear();
          
              queryIOExp.addRecord();
              queryIOExp.setFieldValue("solver", solver->getClassName());
              queryIOExp.setFieldValue("storageOverheadThreshold", boost::str(boost::format("%.2f") % sot));
              queryIOExp.setFieldValue("io", io.at(j).getMean());
              queryIOExp.setFieldValue("deviation", io.at(j).getStandardDeviation());
              io.at(j).clear();
              
              storageExp.addRecord();
              storageExp.setFieldValue("solver", solver->getClassName());
              storageExp.setFieldValue("storageOverheadThreshold", boost::str(boost::format("%.2f") % sot));
              storageExp.setFieldValue("storage", storage.at(j).getMean());    
              storageExp.setFieldValue("deviation", storage.at(j).getStandardDeviation());               
              storage.at(j).clear();
          } else {
              runningTimeExp.addRecord();
              runningTimeExp.setFieldValue("solver", solver->getClassName());
              runningTimeExp.setFieldValue("storageOverheadThreshold", boost::str(boost::format("%.2f") % sot));
              runningTimeExp.setFieldValue("time", "n/a");
              runningTimeExp.setFieldValue("deviation", "n/a");
              times.at(j).clear();
          
              queryIOExp.addRecord();
              queryIOExp.setFieldValue("solver", solver->getClassName());
              queryIOExp.setFieldValue("storageOverheadThreshold", boost::str(boost::format("%.2f") % sot));
              queryIOExp.setFieldValue("io", "n/a");
              queryIOExp.setFieldValue("deviation", "n/a");
              io.at(j).clear();
              
              storageExp.addRecord();
              storageExp.setFieldValue("solver", solver->getClassName());
              storageExp.setFieldValue("storageOverheadThreshold", boost::str(boost::format("%.2f") % sot));
              storageExp.setFieldValue("storage", "n/a");    
              storageExp.setFieldValue("deviation", "n/a");               
              storage.at(j).clear();

              errorFlags.at(j) = false;

          }
          j++;
      }
      cerr << " (" << (completed / total) * 100 << "%)" << endl;           
  }


  for (auto exp : expData) {
      exp->close();
  }

};
