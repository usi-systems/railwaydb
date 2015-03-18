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
        string dbDirPath = "data";
        string expName   = str(boost::format("tweetDB%08d") % blockSize);
        string pathAndName      = str(boost::format("data/tweetDB%08d") % blockSize);
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
    
    ExpSetupHelper::populateGraphFromTweets("data/tweets", graphs_, tsStart_, tsEnd_, vertices_);
    
    std::cout << "Start: " << tsStart_ << std::endl;
    std::cout << "End: " << tsEnd_ << std::endl;

}

void VsBlockSize::tearDown()
{

}

void VsBlockSize::makeEdgeIOCountExp(ExperimentalData * exp) {
  exp->setDescription("Query IO Vs. EdgeIOCount");
  exp->addField("solver");
  exp->addField("blockSize");
  exp->addField("edgeIO");
  exp->addField("deviation");
  exp->setKeepValues(false);
}

void VsBlockSize::makeEdgeWriteIOCountExp(ExperimentalData * exp) {
  exp->setDescription("Storage Overhead Vs. EdgeWriteIOCount");
  exp->addField("solver");
  exp->addField("blockSize");
  exp->addField("edgeWriteIO");
  exp->addField("deviation");
  exp->setKeepValues(false);
}

void VsBlockSize::makeEdgeReadIOCountExp(ExperimentalData * exp) {
    exp->setDescription("Running Time Vs. EdgeReadIOCount");
    exp->addField("solver");
    exp->addField("blockSize");
    exp->addField("edgeReadIO");
    exp->addField("deviation");
    exp->setKeepValues(false);
}

void VsBlockSize::runWorkload(InteractionGraph * graph, std::vector<core::FocusedIntervalQuery> & queries, std::vector<int> indices)
{
    for (int i : indices) {
        graph->processFocusedIntervalQuery(queries[i]);
    }
}


std::vector<int> VsBlockSize::genWorkload(size_t numQueryTypes) 
{
    util::ZipfRand queryGen_(queryZipfParam_, numQueryTypes);
    unsigned seed = time(NULL);
    queryGen_.setSeed(seed++);
    vector<int> indices;
    for (int i = 0; i < numQueries_; ++i) {
        indices.push_back(queryGen_.getRandomValue());
    }
    return indices;
}

void VsBlockSize::process()
{

    SimulationConf simConf;
    double storageOverheadThreshold = 1.0;

    assert(graphs_.size() >= 1);
    simConf.setAttributeCount( graphs_[0]->getConf().getEdgeSchema().getAttributes().size() );
    std::vector<core::FocusedIntervalQuery> queries = simConf.getQueries(graphs_[0].get(), tsStart_, tsEnd_, vertices_);
    std::vector<int> indicies = genWorkload(queries.size()-1);

    runWorkload(graphs_[0].get(),queries, indicies);

    SchemaStats stats = graphs_[0]->getSchemaStats();
    std::map<BucketId,common::QueryWorkload> workloads = graphs_[0]->getWorkloads();

    // TODO: this shouldn't be failing...
    assert(workloads.size() == 1);

    QueryWorkload workload = workloads.begin()->second;





//     // Want to keep the delta large enough so that 
//     // it is not cached
//     // make the cache size a few blocks, pick a time range bigger than
//     // that
//     // Keep the database small as well.
//     // Query range is the entire databae
    

    ExperimentalData edgeIOCountExp("EdgeIOCountVsBlockSize");
    ExperimentalData edgeWriteIOCountExp("EdgeWriteIOCountVsBlockSize");
    ExperimentalData edgeReadIOCountExp("EdgeReadIOCountVsBlockSize");

    auto expData = { &edgeIOCountExp, &edgeIOCountExp, &edgeReadIOCountExp };

    makeEdgeIOCountExp(&edgeIOCountExp);
    makeEdgeWriteIOCountExp(&edgeWriteIOCountExp);
    makeEdgeReadIOCountExp(&edgeReadIOCountExp);
    
    for (auto exp : expData) {
        exp->open();
     }

    auto solvers = {
        SolverFactory::instance().makeSinglePartition(),
        SolverFactory::instance().makeOptimalNonOverlapping()
    };

    vector<util::RunningStat> edgeIO;
    vector<util::RunningStat> edgeWriteIO;
    vector<util::RunningStat> edgeReadIO;
    vector<std::string> names;

     for (auto solver : solvers) {
         edgeIO.push_back(util::RunningStat());
         edgeWriteIO.push_back(util::RunningStat());
         edgeReadIO.push_back(util::RunningStat());
         names.push_back(solver->getClassName());
     }

//     // Create k databases, one for each block size
//     // For each block size, use a different database
//     // for each data base, try the different partitioning schemes
//     // We will only run on non-overlapping
//     // Need to flush the file system cache between each run of the
//     // experiment
//     // right before time.start()
//     // on mac, system(“purge”);

     int j;
     for (auto iter = graphs_.begin(); iter != graphs_.end(); ++iter) {
         for (int i = 0; i < numRuns_; i++) {
             j = 0;
             for (auto solver : solvers) {
                 auto & partIndex = (*iter)->getPartitionIndex();
                 auto origParting = partIndex.getTimeSlicedPartitioning(Timestamp(0.0));
                 intergdb::common::Partitioning solverSolution =
                     solver->solve(workload, storageOverheadThreshold, stats);
                 std::cout << solverSolution.toString() << std::endl;
                 TimeSlicedPartitioning newParting{}; // -inf to inf
                 newParting.getPartitioning() = solverSolution.toStringSet();
                 partIndex.replaceTimeSlicedPartitioning(origParting, {newParting});
                 runWorkload((*iter).get(),queries, indicies);

                 std::cout << (*iter)->getEdgeIOCount() << std::endl;
                 std::cout << (*iter)->getEdgeReadIOCount() << std::endl;
                 std::cout << (*iter)->getEdgeWriteIOCount() << std::endl;

                 edgeIO[j].push((*iter)->getEdgeIOCount());
                 edgeWriteIO[j].push((*iter)->getEdgeReadIOCount());
                 edgeReadIO[j].push((*iter)->getEdgeWriteIOCount());
                 j++;
             }
         }
        j = 0;
/*
        for (auto solver : solvers) {

          edgeIOCountExp.addRecord();
          edgeIOCountExp.setFieldValue("solver", solver->getClassName());
          edgeIOCountExp.setFieldValue("blockSize", blockSize);
          edgeIOCountExp.setFieldValue("time", edgeIO.at(j).getMean());
          edgeIOCountExp.setFieldValue("deviation", edgeIO.at(j).getStandardDeviation());
          edgeIO.at(j).clear();

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
*/
     }

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
