#include <intergdb/exp/Experiments.h>
#include <intergdb/expcommon/ExperimentalData.h>
#include <intergdb/util/RunningStat.h>
#include <intergdb/util/AutoTimer.h>
#include <intergdb/common/Cost.h>
#include <intergdb/common/SchemaStats.h>
#include <intergdb/optimizer/Solver.h>
#include <intergdb/optimizer/SolverFactory.h>
#include <intergdb/core/InteractionGraph.h>

#include <boost/filesystem.hpp>
#include <iostream>
#include <random>
#include <vector>

using namespace std;
using namespace intergdb;
using namespace intergdb::core;
using namespace intergdb::common;
using namespace intergdb::expcommon;
using namespace intergdb::exp;
using namespace intergdb::optimizer;


void VsBlockSize::setUp()
{
    cout << " VsBlockSize::setUp()" << endl;
    auto storageDir = boost::filesystem::unique_path("/tmp/mydb_%%%%");
    conf.reset(new Conf("testDB", storageDir.string(),
                        {{"v", DataType::STRING}}, // vertex schema
                        {{"a", DataType::STRING}, {"b", DataType::STRING}})); // edge schema
    if (boost::filesystem::exists(conf->getStorageDir()))
        boost::filesystem::remove_all(conf->getStorageDir());
    boost::filesystem::create_directories(conf->getStorageDir());
    graph.reset(new InteractionGraph(*conf));
}

void VsBlockSize::tearDown()
{
    boost::filesystem::remove_all(graph->getConf().getStorageDir());
}


void VsBlockSize::makeQueryIOExp(ExperimentalData * exp) {
  exp->setDescription("Query IO Vs. BlockSize");
  exp->addField("solver");
  exp->addField("attributes");
  exp->addField("io");
  exp->addField("deviation");
  exp->setKeepValues(false);
}

void VsBlockSize::makeStorageExp(ExperimentalData * exp) {
  exp->setDescription("Storage Overhead Vs. BlockSize");
  exp->addField("solver");
  exp->addField("attributes");
  exp->addField("storage");
  exp->addField("deviation");
  exp->setKeepValues(false);
}

void VsBlockSize::makeRunningTimeExp(ExperimentalData * exp) {
    exp->setDescription("Running Time Vs. BlockSize");
    exp->addField("solver");
    exp->addField("blockSize");
    exp->addField("time");
    exp->addField("deviation");
    exp->setKeepValues(false);
}

void VsBlockSize::runWorkload(InteractionGraph * graph)
{
    FocusedIntervalQuery fiq(2, 5.0, 10.0, {"a", "b"});
    InteractionGraph::EdgeIterator fiqIt = graph->processFocusedIntervalQuery(fiq);
    fiqIt.next();
}

void VsBlockSize::createGraph()
{
    // Create a graph...
    graph->createVertex(2, "v2");
    graph->createVertex(4, "v4");
    Timestamp ts = 7.0;
    graph->addEdge(2, 4, ts, "a-data", "b-data");
    graph->flush();
    graph.reset(NULL); // why does this work?
    sleep(1); // wait for file locks to be released
}

TimeSlicedPartitioning VsBlockSize::convertPartitioning(intergdb::common::Partitioning p) {
    TimeSlicedPartitioning newParting{}; // -inf to inf
    newParting.getPartitioning() = { {"a"}, {"b"} };
    return newParting;
}

void VsBlockSize::process() 
{

    util::AutoTimer timer;  

    createGraph();

    // Conf * conf = graph->getConf()

    graph.reset(new InteractionGraph(*conf));

    QueryWorkload workload;    
    double storageOverheadThreshold = 1.0;
    SchemaStats stats;

    int numRuns = 1;

    ExperimentalData queryIOExp("QueryIOVsBlockSize");
    ExperimentalData runningTimeExp("RunningTimeVsBlockSize");
    ExperimentalData storageExp("StorageOverheadVsBlockSize");
    
    auto expData = { &queryIOExp, &runningTimeExp, &storageExp };

    makeQueryIOExp(&queryIOExp);
    makeRunningTimeExp(&runningTimeExp);
    makeStorageExp(&storageExp);
    
    for (auto exp : expData) {
        exp->open();
    }
    
    auto solvers = {       
        SolverFactory::instance().makeSinglePartition(), 
        SolverFactory::instance().makeOptimalOverlapping() 
    };

    auto blockSizes = {2, 4, 6, 8, 10, 12, 14, 16 }; 
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
    for (double blockSize : blockSizes) {
        for (int i = 0; i < numRuns; i++) {
            // set the block size       
            j = 0;
            for (auto solver : solvers) {     
                auto & partIndex = graph->getPartitionIndex();
                auto origParting = partIndex.getTimeSlicedPartitioning(Timestamp(0.0));
                TimeSlicedPartitioning newParting = 
                    convertPartitioning(solver->solve(workload, storageOverheadThreshold, stats)); 
                partIndex.replaceTimeSlicedPartitioning(origParting, {newParting});
                timer.start();
                // run workload
                runWorkload(graph.get());                    
                // io.at(j).push(cost.getIOCost(partitioning, workload));
                // storage.at(j).push(cost.getStorageOverhead(partitioning, workload));   
                // times.at(j).push(timer.getRealTimeInSeconds());                           
                j++;
                timer.stop();
                std::cout << "Workload took: " << timer.getRealTimeInSeconds() << std::endl;
            }
        }
        j = 0;
        for (auto solver : solvers) {  
   
          runningTimeExp.addRecord();
          runningTimeExp.setFieldValue("solver", solver->getClassName());
          runningTimeExp.setFieldValue("blockSize", blockSize);
          runningTimeExp.setFieldValue("time", times.at(j).getMean());
          runningTimeExp.setFieldValue("deviation", times.at(j).getStandardDeviation());
          times.at(j).clear();
          
          queryIOExp.addRecord();
          queryIOExp.setFieldValue("solver", solver->getClassName());
          queryIOExp.setFieldValue("blockSize", blockSize);        
          queryIOExp.setFieldValue("io", io.at(j).getMean());
          queryIOExp.setFieldValue("deviation", io.at(j).getStandardDeviation());
          io.at(j).clear();
          
          storageExp.addRecord();
          storageExp.setFieldValue("solver", solver->getClassName());
          storageExp.setFieldValue("blockSize",blockSize);
          storageExp.setFieldValue("storage", storage.at(j).getMean());    
          storageExp.setFieldValue("deviation", storage.at(j).getStandardDeviation());               
          storage.at(j).clear();

          j++;
      }
    }

    for (auto exp : expData) {
        exp->close();
    }


/*
  cerr << "This is an experiment with name: " 
    << this->getClassName() << endl;
  

  SimulationConf simConf;
  SchemaStats stats;
  Cost cost(stats);
  util::AutoTimer timer;  
*/

};
