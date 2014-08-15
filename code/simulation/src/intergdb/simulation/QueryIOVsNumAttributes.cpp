#include <intergdb/simulation/Experiments.h>
#include <intergdb/simulation/ExperimentalData.h>
#include <intergdb/simulation/SimulationConf.h>

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

void QueryIOVsNumAttributes::process() 
{
  cerr << "This is a sample experiment with name: " 
    << this->getClassName() << endl;
  
  SimulationConf simConf;
  Cost cost;
  QueryWorkload workload = simConf.getQueryWorkload();
  double storageOverheadThreshold = 0.0;
  cerr << workload.toString() << endl;

  mt19937 rgen;
  uniform_real_distribution<> udist(0, 10);

  ExperimentalData exp(getClassName());
  exp.setDescription("This experiment compares the query io vs. the number of attributes for each of the partitioning methods.");

  exp.addField("solver");
  exp.addField("attributes");
  exp.addField("io");


  exp.setKeepValues(false);
  exp.open();

/*
  for (size_t block_size : {1024, 2048, 4096}) {
    for (string const & approach : {"random", "smart", "optimal"}) {
      exp.addRecord();
      exp.setFieldValue("block_size", block_size);
      exp.setFieldValue("approach", approach);
      double locality = udist(rgen);
      exp.setFieldValue("locality", locality);
    }
  }
*/


  for (auto solver : { SolverFactory::instance().makeSinglePartition(), 
                       SolverFactory::instance().makePartitionPerAttribute()}) {
      cerr << "Running with solver: "
           << solver->getClassName() << endl;

      Partitioning partitioning = solver->solve(workload, storageOverheadThreshold); 
      exp.addRecord();
      exp.setFieldValue("solver", solver->getClassName());
      exp.setFieldValue("attributes", workload.getAttributes().size());
      exp.setFieldValue("io", cost.getIOCost(partitioning, workload));
  }

  exp.close();
};
