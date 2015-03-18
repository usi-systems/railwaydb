#include <intergdb/simulation/Experiments.h>
#include <intergdb/simulation/ExperimentalData.h>
#include <intergdb/simulation/ExpSetupHelper.h>
#include <intergdb/simulation/SimulationConf.h>

#include <intergdb/util/RunningStat.h>
#include <intergdb/util/AutoTimer.h>
#include <intergdb/common/Cost.h>
#include <intergdb/common/SchemaStats.h>
#include <intergdb/optimizer/Solver.h>
#include <intergdb/optimizer/SolverFactory.h>
#include <intergdb/core/InteractionGraph.h>

#include <boost/filesystem.hpp>
#include <boost/format.hpp> 
#include <iostream>
#include <random>
#include <vector>

using namespace std;
using namespace intergdb;
using namespace intergdb::core;
using namespace intergdb::common;
using namespace intergdb::optimizer;
using namespace intergdb::simulation;

VsBlockSize::VsBlockSize() { }

void VsBlockSize::printTweets()
{
    uint64_t tsStart;
    uint64_t tsEnd;

    ExpSetupHelper::scanTweets("data/tweets", [&] (uint64_t time,
                                                   int64_t from, vector<int64_t> const& tos, Tweet const& tweet)
    {
        for (auto const& to : tos)
            cerr << time << ", " << from << " -> " << to
                 << ", tweet: " << tweet << endl;
    }, tsStart, tsEnd);
}

void VsBlockSize::setUp()
{
    cout << " VsBlockSize::setUp()" << endl;


    for (size_t blockSize : blockSizes_) {
        std::cout << "block size " << blockSize << std::endl;       
        string dbDirPath = str(boost::format("data%08d") % blockSize);
        string expName   = str(boost::format("tweetDB%08d") % blockSize);
        string pathAndName      = str(boost::format("data%08d/tweetDB%08d") % blockSize % blockSize);
        std::cout << dbDirPath << std::endl;
        std::cout << expName << std::endl;
        std::cout << pathAndName << std::endl;

        // Create tweetDB if its not there
        if( !(boost::filesystem::exists(pathAndName))) {
            boost::filesystem::create_directory(pathAndName);
        }

        // Clean up anything that is in the directory
        boost::filesystem::path path_to_remove(pathAndName);
        for (boost::filesystem::directory_iterator end_dir_it, it(path_to_remove); it!=end_dir_it; ++it) {
            remove_all(it->path());
        }

        // Create the graph conf, one for each block size
        Conf conf = ExpSetupHelper::createGraphConf(dbDirPath, expName);
        conf.blockSize() = blockSize;
        confs_.push_back(conf);

        // Create a graph for each block size
        std::unique_ptr<core::InteractionGraph> graph;            
        graph.reset(new InteractionGraph(conf));
        graphs_.push_back(std::move(graph));

    }

    
    // ExpSetupHelper::populateGraphFromTweets("data/tweets", *graph, tsStart_, tsEnd_, vertices_);
    // std::cout << "Start: " << tsStart_ << std::endl;
    // std::cout << "End: " << tsEnd_ << std::endl;

}

void VsBlockSize::tearDown()
{

}


void VsBlockSize::makeQueryIOExp(ExperimentalData * exp) {
  exp->setDescription("Query IO Vs. BlockSize");
  exp->addField("solver");
  exp->addField("blockSize");
  exp->addField("io");
  exp->addField("deviation");
  exp->setKeepValues(false);
}

void VsBlockSize::makeStorageExp(ExperimentalData * exp) {
  exp->setDescription("Storage Overhead Vs. BlockSize");
  exp->addField("solver");
  exp->addField("blockSize");
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

void VsBlockSize::runWorkload(InteractionGraph * graph, std::vector<core::FocusedIntervalQuery> & queries)
{
    for (auto q : queries) {
        std::cout << q.toString() << std::endl;
        graph->processFocusedIntervalQuery(q);
    }
}

void VsBlockSize::process()
{


    SimulationConf simConf;
//     // graph.reset(new InteractionGraph(*conf));

//     simConf.setAttributeCount( graph->getConf().getEdgeSchema().getAttributes().size() );
//     std::vector<core::FocusedIntervalQuery> queries = simConf.getQueries(graph.get(), tsStart_, tsEnd_, vertices_);

//     for (auto q : queries) {
//         std::cout << q.toString() << std::endl;
//         graph->processFocusedIntervalQuery(q);
//     }

//     SchemaStats stats = graph->getSchemaStats();
//     std::map<BucketId,common::QueryWorkload> workloads = graph->getWorkloads();

//     for (auto& x: workloads) {
//         std::cout << "-> " << x.first << std::endl; //" => " << x.second << '\n';
//     }  

//     QueryWorkload workload = workloads.begin()->second;


//     double storageOverheadThreshold = 1.0;
//     Cost cost(stats);
//     util::AutoTimer timer;

//     int numRuns = 1;

//     // TODO:
//     // Repeatedly call zipf.random
//     // from the set of queries for 100 times...

//     // int meanQueryIntervalSize;


//     // use a fixed start and end 
//     // start time of graph + some delta (30 min)
//     // Want to keep the delta large enough so that 
//     // it is not cached
//     // make the cache size a few blocks, pick a time range bigger than
//     // that
//     // Keep the database small as well.
//     // Query range is the entire databae
    



//     ExperimentalData queryIOExp("QueryIOVsBlockSize");
//     ExperimentalData runningTimeExp("RunningTimeVsBlockSize");
//     ExperimentalData storageExp("StorageOverheadVsBlockSize");

//     auto expData = { &queryIOExp, &runningTimeExp, &storageExp };

//     makeQueryIOExp(&queryIOExp);
//     makeRunningTimeExp(&runningTimeExp);
//     makeStorageExp(&storageExp);

//     for (auto exp : expData) {
//         exp->open();
//     }

//     auto solvers = {
//         SolverFactory::instance().makeSinglePartition(),
//         SolverFactory::instance().makeOptimalNonOverlapping()
//     };

//     vector<util::RunningStat> io;
//     vector<util::RunningStat> storage;
//     vector<util::RunningStat> times;
//     vector<std::string> names;


//     for (auto solver : solvers) {
//         io.push_back(util::RunningStat());
//         storage.push_back(util::RunningStat());
//         times.push_back(util::RunningStat());
//         names.push_back(solver->getClassName());
//         vector<std::string> names;
//     }

//     // Create k databases, one for each block size
//     // For each block size, use a different database
//     // for each data base, try the different partitioning schemes
//     // We will only run on non-overlapping
//     // Need to flush the file system cache between each run of the
//     // experiment
//     // right before time.start()
//     // on mac, system(“purge”);

//     int j;
//     for (double blockSize : blockSizes_) {
//         for (int i = 0; i < numRuns; i++) {
//             // set the block size
//             j = 0;
//             for (auto solver : solvers) {
//                 auto & partIndex = graph->getPartitionIndex();
//                 auto origParting = partIndex.getTimeSlicedPartitioning(Timestamp(0.0));

//                 intergdb::common::Partitioning solverSolution =
//                     solver->solve(workload, storageOverheadThreshold, stats);

//                 std::cout << ">>>>>" << std::endl;

//                 std::cout << solverSolution.toString() << std::endl;

//                 std::cout << "<<<<<" << std::endl;

//                 TimeSlicedPartitioning newParting{}; // -inf to inf

//                 newParting.getPartitioning() = solverSolution.toStringSet();

//                 partIndex.replaceTimeSlicedPartitioning(origParting, {newParting});

//                 std::cout << "A" << std::endl;


//                 timer.start();
//                 // run workload
//                 //runWorkload(graph.get());
//                 io.at(j).push(cost.getIOCost(solverSolution, workload));
//                 storage.at(j).push(cost.getStorageOverhead(solverSolution, workload));
//                 times.at(j).push(timer.getRealTimeInSeconds());


//                 // use iteractiongraph.getEdgeIOCount()
//                 // use iteractiongraph.getEdgeReadIOCount()
//                 // use iteractiongraph.getEdgeWriteIOCount()

//                 // Look at how much data is in the directory? It should match the model.
                

//                 j++;
//                 timer.stop();
//                 std::cout << "Workload took: " << timer.getRealTimeInSeconds() << std::endl;
//             }
//         }
//         j = 0;
//         for (auto solver : solvers) {

//           runningTimeExp.addRecord();
//           runningTimeExp.setFieldValue("solver", solver->getClassName());
//           runningTimeExp.setFieldValue("blockSize", blockSize);
//           runningTimeExp.setFieldValue("time", times.at(j).getMean());
//           runningTimeExp.setFieldValue("deviation", times.at(j).getStandardDeviation());
//           times.at(j).clear();

//           queryIOExp.addRecord();
//           queryIOExp.setFieldValue("solver", solver->getClassName());
//           queryIOExp.setFieldValue("blockSize", blockSize);
//           queryIOExp.setFieldValue("io", io.at(j).getMean());
//           queryIOExp.setFieldValue("deviation", io.at(j).getStandardDeviation());
//           io.at(j).clear();

//           storageExp.addRecord();
//           storageExp.setFieldValue("solver", solver->getClassName());
//           storageExp.setFieldValue("blockSize",blockSize);
//           storageExp.setFieldValue("storage", storage.at(j).getMean());
//           storageExp.setFieldValue("deviation", storage.at(j).getStandardDeviation());
//           storage.at(j).clear();

//           j++;
//       }
//     }

//     for (auto exp : expData) {
//         exp->close();
//     }


// /*
//   cerr << "This is an experiment with name: "
//     << this->getClassName() << endl;


//   SimulationConf simConf;
//   SchemaStats stats;
//   Cost cost(stats);
//   util::AutoTimer timer;
// */

};
