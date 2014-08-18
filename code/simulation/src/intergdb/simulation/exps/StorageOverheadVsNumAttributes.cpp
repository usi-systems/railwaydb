#include <intergdb/simulation/Experiments.h>
#include <intergdb/simulation/ExperimentalData.h>
#include <intergdb/simulation/SimulationConf.h>
#include <intergdb/util/RunningStat.h>

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

void StorageOverheadVsNumAttributes::process() 
{

    cerr << "This is an experiment with name: " 
         << this->getClassName() << endl;
    
    SimulationConf simConf;
    Cost cost;
    double storageOverheadThreshold = numeric_limits<double>::max( );
    ExperimentalData exp(getClassName());
    exp.setDescription("StorageOverhead Vs. NumAttributes");
    exp.addField("solver");
    exp.addField("attributes");
    exp.addField("storage");
    exp.setKeepValues(false);
    exp.open();

    int numRuns = 3;
    auto solvers = { SolverFactory::instance().makeSinglePartition(), 
                     SolverFactory::instance().makePartitionPerAttribute(),
                     SolverFactory::instance().makeOptimalOverlapping(), 
                     SolverFactory::instance().makeOptimalNonOverlapping() };

    auto attributeCounts = {10, 20, 30, 40, 50};
    util::RunningStat storage;

    for (auto solver : solvers) {
        for (int attributeCount : attributeCounts) {
            simConf.setAttributeCount(attributeCount);
            QueryWorkload workload = simConf.getQueryWorkload();            
            for (int i = 0; i < numRuns; i++) {
                Partitioning partitioning = solver->solve(workload, storageOverheadThreshold);              
                storage.push(cost.getStorageOverhead(partitioning, workload));
            }         
            exp.addRecord();
            exp.setFieldValue("solver", solver->getClassName());
            exp.setFieldValue("attributes", workload.getAttributes().size());
            exp.setFieldValue("storage", storage.getMean());          
            storage.clear();
        }
    }
  
    exp.close();

};
