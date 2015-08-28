#include <intergdb/common/Cost.h>
#include <intergdb/common/SchemaStats.h>
#include <intergdb/core/InteractionGraph.h>
#include <intergdb/optimizer/Solver.h>
#include <intergdb/optimizer/SolverFactory.h>
#include <intergdb/simulation/ExperimentalData.h>
#include <intergdb/simulation/Experiments.h>
#include <intergdb/simulation/ExpSetupHelper.h>
#include <intergdb/simulation/SimulationConf.h>
#include <intergdb/util/AutoTimer.h>
#include <intergdb/util/RunningStat.h>

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

VsTimeDeltaDFS::VsTimeDeltaDFS() { }

void VsTimeDeltaDFS::setUp()
{
    cout << " VsTimeDeltaDFS::setUp()..." << endl;
    
    string dbDirPath = "data";
    string expName = str(boost::format("tweetDB%08d") % blockSize_);
    string pathAndName =
        str(boost::format("data/tweetDB%08d") % blockSize_);

    // Create tweetDB if its not there
    if( !(boost::filesystem::exists(pathAndName))) {
        boost::filesystem::create_directory(pathAndName);
    }
    
    // Clean up anything that is in the directory
    boost::filesystem::path path_to_remove(pathAndName);
    for (boost::filesystem::directory_iterator end_dir_it,
             it(path_to_remove); it!=end_dir_it; ++it)
    {
        remove_all(it->path());
    }
    
    // Create the graph conf
    conf_.reset(new Conf(ExpSetupHelper::createGraphConf(dbDirPath, expName)));
    conf_->setBlockSize(blockSize_);
    conf_->setBlockBufferSize(blockBufferSize_);

    // Create a graph 
    graph_.reset(new InteractionGraph(*conf_));
   
    // // Create a vertex just so we can populate it with
    // // the same function.
    std::vector< std::unique_ptr<core::InteractionGraph> > graphs;
    graphs.push_back(std::move(graph_));

    cout << " populateGraphFromTweets..." << endl;
    ExpSetupHelper::populateGraphFromTweets(
        "data/tweets", graphs, tsStart_, tsEnd_, vertices_);
    cout << " done." << endl;

    graph_ = std::move(graphs.back());
    graphs.pop_back();

    std::cout << "start " << tsStart_ << std::endl;
    std::cout << "stop " << tsEnd_ << std::endl;

}

void VsTimeDeltaDFS::tearDown()
{

}

void VsTimeDeltaDFS::makeEdgeIOCountExp(ExperimentalData * exp)
{
    exp->setDescription("Time Delta Vs. EdgeIOCount");
    exp->addField("solver");
    exp->addField("delta");
    exp->addField("edgeIO");
    exp->addField("deviation");
    exp->setKeepValues(false);
}

void VsTimeDeltaDFS::makeEdgeWriteIOCountExp(ExperimentalData * exp)
{
    exp->setDescription("Time Delta Vs. EdgeWriteIOCount");
    exp->addField("solver");
    exp->addField("delta");
    exp->addField("edgeWriteIO");
    exp->addField("deviation");
    exp->setKeepValues(false);
}

void VsTimeDeltaDFS::makeEdgeReadIOCountExp(ExperimentalData * exp) {
    exp->setDescription("Time Delta Vs. EdgeReadIOCount");
    exp->addField("solver");
    exp->addField("delta");
    exp->addField("edgeReadIO");
    exp->addField("deviation");
    exp->setKeepValues(false);
}

void VsTimeDeltaDFS::makeRunningTimeExp(ExperimentalData * exp)
{
    exp->setDescription("Time Delta Vs. RunningTime");
    exp->addField("solver");
    exp->addField("delta");
    exp->addField("time");
    exp->addField("deviation");
    exp->setKeepValues(false);
}

void VsTimeDeltaDFS::process()
{
    SimulationConf simConf;
    double storageOverheadThreshold = 1.0;
    util::AutoTimer timer;

    assert(graph_ != NULL);
    simConf.setAttributeCount(
        graph_->getConf().getEdgeSchema().getAttributes().size());

    simConf.setQueryTypeCount(numQueryTemplates_);

    ExperimentalData edgeIOCountExp("EdgeIOCountVsTimeDeltaDFS");
    ExperimentalData edgeWriteIOCountExp("EdgeWriteIOCountVsTimeDeltaDFS");
    ExperimentalData edgeReadIOCountExp("EdgeReadIOCountVsTimeDeltaDFS");
    ExperimentalData runningTimeExp("RunningTimeVsTimeDeltaDFS");

    auto expData =
        { &edgeIOCountExp, &edgeWriteIOCountExp, &edgeReadIOCountExp, &runningTimeExp };

    makeEdgeIOCountExp(&edgeIOCountExp);
    makeEdgeWriteIOCountExp(&edgeWriteIOCountExp);
    makeEdgeReadIOCountExp(&edgeReadIOCountExp);
    makeRunningTimeExp(&runningTimeExp);

    for (auto exp : expData)
        exp->open();

    vector<util::RunningStat> edgeIO;
    vector<util::RunningStat> edgeWriteIO;
    vector<util::RunningStat> edgeReadIO;
    vector<util::RunningStat> times;
        
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
        times.push_back(util::RunningStat());
        names.push_back(solver->getClassName());
    }

    int solverIndex;
    size_t prevEdgeIOCount;
    size_t prevEdgeReadIOCount;
    size_t prevEdgeWriteIOCount;
    
    std::cout << "Running experiments..." << std::endl;
  
    int deltaIndex = -1;

    SchemaStats stats = graph_->getSchemaStats();

    std::cout << stats.toString() << std::endl;

    for (auto delta : timeDeltas_) {
        deltaIndex++;
        for (int i = 0; i < numRuns_; i++) {
            
            // For each run with a different delta, generate a different set of queries:
            
            // generate a different workload with numQueryTemplates
            std::vector<std::vector<std::string> > templates =
                simConf.getQueryTemplates(graph_.get());

            std::vector<core::FocusedIntervalQuery> queries = 
                ExpSetupHelper::genSearchQueries(templates,
                                                 queryZipfParam_, 
                                                 numQueries_,
                                                 tsStart_,
                                                 tsEnd_, 
                                                 delta, 
                                                 vertices_);

            graph_->resetWorkloads();

            ExpSetupHelper::runDFS(graph_.get(), queries);
            
            std::map<BucketId,common::QueryWorkload> ws =
                graph_->getWorkloads();
            // Make sure everything is in one bucket
            assert(ws.size() == 1);
            QueryWorkload workload = ws.begin()->second;
            
            solverIndex = -1;
            for (auto solver : solvers) {
                solverIndex++;

                auto & partIndex = graph_->getPartitionIndex();
                auto origParting =
                    partIndex.getTimeSlicedPartitioning(Timestamp(0.0));
                intergdb::common::Partitioning solverSolution =
                    solver->solve(workload, storageOverheadThreshold, stats);
                
                std::cout << "Solver: " <<  solver->getClassName() << std::endl;
                std::cout << "numRuns: " << i << std::endl;
                std::cout << "delta: " << delta << std::endl;
                
                std::cout << solverSolution.toString() << std::endl;
                TimeSlicedPartitioning newParting{}; // -inf to inf
                newParting.getPartitioning() = solverSolution.toStringSet();
                partIndex.replaceTimeSlicedPartitioning(
                    origParting, {newParting});
                
                prevEdgeIOCount = graph_->getEdgeIOCount();
                prevEdgeReadIOCount = graph_->getEdgeReadIOCount();
                prevEdgeWriteIOCount = graph_->getEdgeWriteIOCount();

                ExpSetupHelper::purge();
                graph_.reset(new InteractionGraph(*conf_));
                timer.start();
                ExpSetupHelper::runDFS(graph_.get(),queries);
                timer.stop();


                std::cout << "getEdgeIOCount: " << 
                    graph_->getEdgeIOCount() - prevEdgeIOCount << std::endl;
                std::cout << "getEdgeReadIOCount: " << 
                    graph_->getEdgeReadIOCount() - prevEdgeReadIOCount
                          << std::endl;
                std::cout << "getEdgeWriteIOCount: " << 
                    graph_->getEdgeWriteIOCount() - prevEdgeWriteIOCount
                          << std::endl;

                edgeIO[solverIndex].push(
                    graph_->getEdgeIOCount() - prevEdgeIOCount);
                edgeReadIO[solverIndex].push(
                    graph_->getEdgeReadIOCount() - prevEdgeReadIOCount);
                edgeWriteIO[solverIndex].push(
                    graph_->getEdgeWriteIOCount() - prevEdgeWriteIOCount);
                times[solverIndex].push( timer.getRealTimeInSeconds());
            }
        }
    

        for (int solverIndex = 0; solverIndex < solvers.size(); solverIndex++)
        {

            edgeIOCountExp.addRecord();
            edgeIOCountExp.setFieldValue(
                "solver", solvers[solverIndex]->getClassName());
            edgeIOCountExp.setFieldValue(
                "delta",
                boost::lexical_cast<std::string>(timeDeltas_[deltaIndex]));
            edgeIOCountExp.setFieldValue(
                "edgeIO", edgeIO[solverIndex].getMean());
            edgeIOCountExp.setFieldValue(
                "deviation", edgeIO[solverIndex].getStandardDeviation());
            edgeIO[solverIndex].clear();

            edgeWriteIOCountExp.addRecord();
            edgeWriteIOCountExp.setFieldValue(
                "solver", solvers[solverIndex]->getClassName());
            edgeWriteIOCountExp.setFieldValue("delta",
                                              boost::lexical_cast<std::string>(timeDeltas_[deltaIndex]));
            edgeWriteIOCountExp.setFieldValue(
                "edgeWriteIO", edgeWriteIO[solverIndex].getMean());
            edgeWriteIOCountExp.setFieldValue(
                "deviation", edgeWriteIO[solverIndex].getStandardDeviation());
            edgeWriteIO[solverIndex].clear();

            edgeReadIOCountExp.addRecord();
            edgeReadIOCountExp.setFieldValue(
                "solver", solvers[solverIndex]->getClassName());
            edgeReadIOCountExp.setFieldValue(
                "delta",
                boost::lexical_cast<std::string>(timeDeltas_[deltaIndex]));
            edgeReadIOCountExp.setFieldValue(
                "edgeReadIO", edgeReadIO[solverIndex].getMean());
            edgeReadIOCountExp.setFieldValue(
                "deviation", edgeReadIO[solverIndex].getStandardDeviation());
            edgeReadIO[solverIndex].clear();

            runningTimeExp.addRecord();
            runningTimeExp.setFieldValue("solver", solvers[solverIndex]->getClassName());
            runningTimeExp.setFieldValue(
                "delta",
                boost::lexical_cast<std::string>(timeDeltas_[deltaIndex]));                      
            runningTimeExp.setFieldValue("time", times[solverIndex].getMean());
            runningTimeExp.setFieldValue(
                "deviation", times[solverIndex].getStandardDeviation());
            times[solverIndex].clear();
        }
    }

    // std::cout << "done." << std::endl;

     
    for (auto exp : expData)
        exp->close();
};
