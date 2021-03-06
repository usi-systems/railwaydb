#include <intergdb/common/Cost.h>
#include <intergdb/common/SchemaStats.h>
#include <intergdb/optimizer/Solver.h>
#include <intergdb/optimizer/SolverFactory.h>
#include <intergdb/simulation/Constants.h>
#include <intergdb/simulation/ExperimentalData.h>
#include <intergdb/simulation/Experiments.h>
#include <intergdb/simulation/SimulationConf.h>
#include <intergdb/util/AutoTimer.h>
#include <intergdb/util/RunningStat.h>

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <random>
#include <vector>

using namespace boost;
using namespace std;
using namespace intergdb;
using namespace intergdb::common;
using namespace intergdb::simulation;
using namespace intergdb::optimizer;

void VsNumQueryKinds::makeQueryIOExp(ExperimentalData * exp)
{
    exp->setDescription("Query IO Vs. NumQueryKinds");
    exp->addField("solver");
    exp->addField("queryTypeCount");
    exp->addField("io");
    exp->addField("deviation");
    exp->setKeepValues(false);
}

void VsNumQueryKinds::makeStorageExp(ExperimentalData * exp)
{
    exp->setDescription("Storage Overhead Vs. NumQueryKinds");
    exp->addField("solver");
    exp->addField("queryTypeCount");
    exp->addField("storage");
    exp->addField("deviation");
    exp->setKeepValues(false);
}

void VsNumQueryKinds::makeRunningTimeExp(ExperimentalData * exp)
{
    exp->setDescription("Running Time Vs. NumQueryKinds");
    exp->addField("solver");
    exp->addField("queryTypeCount");
    exp->addField("time");
    exp->addField("deviation");
    exp->setKeepValues(false);
}

void VsNumQueryKinds::process()
{
    cerr << "This is an experiment with name: "
         << this->getClassName() << endl;

    SimulationConf simConf;
    SchemaStats stats;
    Cost cost(stats);
    util::AutoTimer timer;

    ExperimentalData queryIOExp("QueryIOVsNumQueryKinds");
    ExperimentalData runningTimeExp("RunningTimeVsNumQueryKinds");
    ExperimentalData storageExp("StorageOverheadVsNumQueryKinds");

    auto expData = { &queryIOExp, &runningTimeExp, &storageExp };

    makeQueryIOExp(&queryIOExp);
    makeRunningTimeExp(&runningTimeExp);
    makeStorageExp(&storageExp);

    for (auto exp : expData)
        exp->open();

    auto solvers = {
        SolverFactory::instance().makeSinglePartition(),
        SolverFactory::instance().makePartitionPerAttribute(),
        //SolverFactory::instance().makeOptimalOverlapping(),
        //SolverFactory::instance().makeOptimalNonOverlapping(),
        SolverFactory::instance().makeHeuristicOverlapping(),
        SolverFactory::instance().makeHeuristicNonOverlapping()
    };

    //auto queryTypeCounts = {2, 4, 6, 8, 10, 12, 14, 16 };
    auto queryTypeCounts = {32, 48, 64, 80, 96, 112, 128};

    
    double total = solvers.size()
        * queryTypeCounts.size()
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
    for (int queryTypeCount : queryTypeCounts) {
        for (int i = 0; i < numRuns; i++) {
            simConf.setQueryTypeCount(queryTypeCount);
            vector<std::unique_ptr<Attribute>> allAttributes;
            auto workloadAndStats =
                simConf.getQueryWorkloadAndStats(allAttributes);
            QueryWorkload workload = workloadAndStats.first;
            stats = workloadAndStats.second;
            j = 0;
            for (auto solver : solvers) {
                timer.start();
                Partitioning partitioning = solver->solve(
                    workload, storageOverheadThreshold, stats);
                timer.stop();
                io.at(j).push(cost.getIOCost(partitioning, workload));
                storage.at(j).push(cost.getStorageOverhead(
                    partitioning, workload));
                times.at(j).push(
                    timer.getRealTimeInSeconds());
                j++;
                cerr << ".";
                completed++;
            }
        }

        int j = 0;
        for (auto solver : solvers) {

            runningTimeExp.addRecord();
            runningTimeExp.setFieldValue("solver", solver->getClassName());
            runningTimeExp.setFieldValue(
                "queryTypeCount", lexical_cast<std::string>(queryTypeCount));
            runningTimeExp.setFieldValue("time", times.at(j).getMean());
            runningTimeExp.setFieldValue(
                "deviation", times.at(j).getStandardDeviation());
            times.at(j).clear();

            queryIOExp.addRecord();
            queryIOExp.setFieldValue("solver", solver->getClassName());
            queryIOExp.setFieldValue(
                "queryTypeCount", lexical_cast<std::string>(queryTypeCount));
            queryIOExp.setFieldValue("io", io.at(j).getMean());
            queryIOExp.setFieldValue(
                "deviation", io.at(j).getStandardDeviation());
            io.at(j).clear();

            storageExp.addRecord();
            storageExp.setFieldValue("solver", solver->getClassName());
            storageExp.setFieldValue(
                "queryTypeCount", lexical_cast<std::string>(queryTypeCount));
            storageExp.setFieldValue("storage", storage.at(j).getMean());
            storageExp.setFieldValue(
                "deviation", storage.at(j).getStandardDeviation());
            storage.at(j).clear();

            j++;
        }
        cerr << " (" << (completed / total) * 100 << "%)" << endl;
    }

    for (auto exp : expData)
        exp->close();
};
