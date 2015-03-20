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
#include <boost/lexical_cast.hpp>

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
    cout << " VsBlockSize::setUp()..." << endl;

    int x = 0;
    int total = blockSizes_.size();

    for (size_t blockSize : blockSizes_) {
        x++;
        std::cout << "    " << x << "/" << total << std::endl;

        string dbDirPath = "data";
        string expName   = str(boost::format("tweetDB%08d") % blockSize);
        string pathAndName      = str(boost::format("data/tweetDB%08d") % blockSize);
      
        // Create tweetDB if its not there
        if( !(boost::filesystem::exists(pathAndName))) {
            boost::filesystem::create_directory(pathAndName);
        }

        // Clean up anything that is in the directory
        //boost::filesystem::path path_to_remove(pathAndName);
        //for (boost::filesystem::directory_iterator end_dir_it, it(path_to_remove); it!=end_dir_it; ++it) {
        //    remove_all(it->path());
        //}

        // Create the graph conf, one for each block size
        Conf conf = ExpSetupHelper::createGraphConf(dbDirPath, expName);
        conf.blockSize() = blockSize;
        conf.blockBufferSize() = blockBufferSize_;
        confs_.push_back(conf);

        // Create a graph for each block size
        std::unique_ptr<core::InteractionGraph> graph;            
        graph.reset(new InteractionGraph(conf));
        graphs_.push_back(std::move(graph));

    }

    cout << " done." << endl;

    //cout << " populateGraphFromTweets..." << endl;
    //ExpSetupHelper::populateGraphFromTweets("data/tweets", graphs_, tsStart_, tsEnd_, vertices_);
    //cout << " done." << endl;

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
        int sum = 0;
        for (auto iqIt = graph->processFocusedIntervalQuery(queries[i]); iqIt.isValid(); iqIt.next()) {
            sum += iqIt.getToVertex();
        }
        assert (sum != 0);
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

    std::vector< std::vector<core::FocusedIntervalQuery> > queries;
    std::vector< std::vector<int> > indicies;
    std::vector< SchemaStats > stats;
    std::vector< QueryWorkload > workloads;

    std::cout << "Generating workload..." << std::endl;
    for (int i=0; i < numRuns_; i++) {
        std::cout << "    " << i << "/" << numRuns_ << std::endl;
        std::vector<core::FocusedIntervalQuery> qs = simConf.getQueries(graphs_[0].get(), tsStart_, tsEnd_, vertices_);
        std::vector<int> inds = genWorkload(qs.size()-1);
        runWorkload(graphs_[0].get(),qs, inds);
        SchemaStats ss = graphs_[0]->getSchemaStats();
        std::map<BucketId,common::QueryWorkload> ws = graphs_[0]->getWorkloads();
        // Make sure everything is in one bucket
        assert(ws.size() == 1);
        QueryWorkload w = ws.begin()->second;

        // (queries, indices, stats, workload)
        queries.push_back(qs);
        indicies.push_back(inds);
        stats.push_back(ss);
        workloads.push_back(w);

        graphs_[0]->resetWorkloads();

    }
    std::cout << "done." << std::endl;

    ExperimentalData edgeIOCountExp("EdgeIOCountVsBlockSize");
    ExperimentalData edgeWriteIOCountExp("EdgeWriteIOCountVsBlockSize");
    ExperimentalData edgeReadIOCountExp("EdgeReadIOCountVsBlockSize");

    auto expData = { &edgeIOCountExp, &edgeWriteIOCountExp, &edgeReadIOCountExp };

    makeEdgeIOCountExp(&edgeIOCountExp);
    makeEdgeWriteIOCountExp(&edgeWriteIOCountExp);
    makeEdgeReadIOCountExp(&edgeReadIOCountExp);
    
    for (auto exp : expData) {
        exp->open();
     }

    vector<util::RunningStat> edgeIO;
    vector<util::RunningStat> edgeWriteIO;
    vector<util::RunningStat> edgeReadIO;
    vector<std::string> names;
    vector< shared_ptr<Solver> > solvers =
        {
            SolverFactory::instance().makeSinglePartition(),
            SolverFactory::instance().makeOptimalNonOverlapping(),
            SolverFactory::instance().makeHeuristicNonOverlapping()
        };


     for (auto solver : solvers) {
         edgeIO.push_back(util::RunningStat());
         edgeWriteIO.push_back(util::RunningStat());
         edgeReadIO.push_back(util::RunningStat());
         names.push_back(solver->getClassName());
     }

     int j;
     size_t prevEdgeIOCount;
     size_t prevEdgeReadIOCount;
     size_t prevEdgeWriteIOCount;
     int x = 0;
     int total = graphs_.size() * numRuns_ * solvers.size();

     std::cout << "Running experiments..." << std::endl;
     for (auto iter = graphs_.begin(); iter != graphs_.end(); ++iter) {      
         for (int i = 0; i < numRuns_; i++) {
             j = 0;
             for (auto solver : solvers) {
                 auto & partIndex = (*iter)->getPartitionIndex();
                 auto origParting = partIndex.getTimeSlicedPartitioning(Timestamp(0.0));
                 intergdb::common::Partitioning solverSolution =
                     solver->solve(workloads[i], storageOverheadThreshold, stats[i]);
                 //std::cout << solverSolution.toString() << std::endl;
                 TimeSlicedPartitioning newParting{}; // -inf to inf
                 newParting.getPartitioning() = solverSolution.toStringSet();
                 partIndex.replaceTimeSlicedPartitioning(origParting, {newParting});
                 // to flush the filesystem cache
                 //system(“purge”);

                 prevEdgeIOCount = (*iter)->getEdgeIOCount();
                 prevEdgeReadIOCount = (*iter)->getEdgeReadIOCount();
                 prevEdgeWriteIOCount = (*iter)->getEdgeWriteIOCount();

                 runWorkload((*iter).get(),queries[i], indicies[i]);

                 
                 std::cout << (*iter)->getEdgeIOCount() - prevEdgeIOCount << std::endl;
                 std::cout << (*iter)->getEdgeReadIOCount() - prevEdgeReadIOCount << std::endl;
                 std::cout << (*iter)->getEdgeWriteIOCount() - prevEdgeWriteIOCount<< std::endl;
                 
                 edgeIO[j].push((*iter)->getEdgeIOCount() - prevEdgeIOCount);
                 edgeReadIO[j].push((*iter)->getEdgeReadIOCount() - prevEdgeReadIOCount);
                 edgeWriteIO[j].push((*iter)->getEdgeWriteIOCount() - prevEdgeWriteIOCount);
                 j++;
                 x++;
                 std::cout << "    " << x << "/" << total << std::endl;         
             }
         }

        for (int j = 0; j < solvers.size(); j++) {

          edgeIOCountExp.addRecord();
          edgeIOCountExp.setFieldValue("solver", solvers[j]->getClassName());
          edgeIOCountExp.setFieldValue("blockSize", boost::lexical_cast<std::string>(blockSizes_[j]));
          edgeIOCountExp.setFieldValue("edgeIO", edgeIO[j].getMean());
          edgeIOCountExp.setFieldValue("deviation", edgeIO[j].getStandardDeviation());
          edgeIO[j].clear();

          edgeWriteIOCountExp.addRecord();
          edgeWriteIOCountExp.setFieldValue("solver", solvers[j]->getClassName());
          edgeWriteIOCountExp.setFieldValue("blockSize", boost::lexical_cast<std::string>(blockSizes_[j]));
          edgeWriteIOCountExp.setFieldValue("edgeWriteIO", edgeWriteIO[j].getMean());
          edgeWriteIOCountExp.setFieldValue("deviation", edgeWriteIO[j].getStandardDeviation());
          edgeWriteIO[j].clear();

          edgeReadIOCountExp.addRecord();
          edgeReadIOCountExp.setFieldValue("solver", solvers[j]->getClassName());
          edgeReadIOCountExp.setFieldValue("blockSize", boost::lexical_cast<std::string>(blockSizes_[j]));
          edgeReadIOCountExp.setFieldValue("edgeReadIO", edgeReadIO[j].getMean());
          edgeReadIOCountExp.setFieldValue("deviation", edgeReadIO[j].getStandardDeviation());
          edgeReadIO[j].clear();

      }

     }

     std::cout << "done." << std::endl;


     for (auto exp : expData) {
         exp->close();
     }

};
