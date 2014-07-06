#include <intergdb/simulation/Experiments.h>
#include <intergdb/simulation/ExperimentalData.h>
#include <intergdb/simulation/SimulationConf.h>

#include <iostream>
#include <random>
#include <vector>

using namespace std;
using namespace intergdb;
using namespace intergdb::simulation;

void SampleExperiment::process() 
{
  cerr << "This is a sample experiment with name: " 
    << this->getClassName() << endl;

  SimulationConf simConf;
  QueryWorkload workload = simConf.getQueryWorkload();
  cerr << workload.toString() << endl;

  mt19937 rgen;
  uniform_real_distribution<> udist(0, 10);

  ExperimentalData exp(getClassName());
  exp.setDescription("This is a sample experiment");
  exp.addField("block_size");
  exp.addField("approach");
  exp.addField("locality");
  exp.setKeepValues(false);
  exp.open();
  for (size_t block_size : {1024, 2048, 4096}) {
    for (string const & approach : {"random", "smart", "optimal"}) {
      exp.addRecord();
      exp.setFieldValue("block_size", block_size);
      exp.setFieldValue("approach", approach);
      double locality = udist(rgen);
      exp.setFieldValue("locality", locality);
    }
  }
  exp.close();
};
