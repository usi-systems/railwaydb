#if false

#include <intergdb/run/exp/RunExp.h>
#include <intergdb/run/AutoTimer.h>
#include <intergdb/run/exp/Experiment.h>
#include <intergdb/run/exp/ExpSetupHelper.h>
#include <intergdb/core/Conf.h>
#include <intergdb/gen/InteractionGraphGenerator.h>

using namespace std;
using namespace intergdb::core;
using namespace intergdb::gen;
using namespace intergdb::run;
using namespace intergdb::run::exp;

void RunQueryTimeForDifferentAlgs::process()
{
    Experiment exp("algorithms_and_query_time");
    exp.setDescription("Query time for graphs with different algs");
    exp.addFieldName("layout_mode");
    exp.addFieldName("candidate_count");
    exp.addFieldName("initial_candidate_selection_policy");
    exp.addFieldName("query_time_extent");
    exp.addFieldName("query_run_time");
    exp.addFieldName("query_io_count");
    exp.addFieldName("query_result_size");
    exp.setKeepValues(false);
    exp.open();

    //size_t vertexDataSize = 10;
    //size_t edgeDataSize = 50;
    double minTime = 0;
    double maxTime = 12 * 60 * 60 * 1000; // 12 hour
    double minTimeExtentMsecs = 16 * 1000; // 16 second
    double maxTimeExtentMsecs = 60 * 60 * 1000; // 1 hour
    size_t nHops = 3, nQueries = 10; // make sure numQueriesToRun*maxTimeExtent < maxTime - minTime
    double queryRunTime = 60.0;

    auto graphType = InteractionGraphGenerator::GraphType::ScaleFree;
    string gtname = InteractionGraphGenerator::getGraphTypeName(graphType);

    for (size_t k=16; k<=16; k*=2) {
        for (size_t p=0; p<Conf::SmartLayoutConf::ICS_nPolicies; ++p) {
            auto icsPolicy = (Conf::SmartLayoutConf::InitialCandidateSelectionPolicy) p;
            string icsPolicyName = Conf::SmartLayoutConf::getInitialCandidateSelectionPolicyName(icsPolicy);
            stringstream name;
            name << gtname << "_variety";
            name << "_l_Smart_k_" << k;
            name << "_p_" << icsPolicyName;
            string dbname = name.str();

            Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
            graphConf.layoutMode() = Conf::LM_Smart;
            graphConf.smartLayoutConf().initialCandidateCount() = k;
            graphConf.smartLayoutConf().initialCandidateSelectionPolicy() = icsPolicy;


            for (double timeExtent=minTimeExtentMsecs; timeExtent<=maxTimeExtentMsecs; timeExtent*=2) {
                vector<core::VertexId> vertices;
                {
                    InteractionGraph<string,string> graph(graphConf);
                    ExpSetupHelper::doVertexQueries(graph, minTime, maxTime, timeExtent, 1, vertices);
                }
                AutoTimer timer;
                size_t totalNumEdges = 0;
                double totalRealTime = 0;
                double totalQueryTime = 0.0;
                size_t totalQueryEdgeIO = 0;
                size_t totalQueryCount = 0;
                while (totalRealTime<queryRunTime) {
                    timer.start();
                    system("purge");
                    InteractionGraph<string,string> graph(graphConf);
                    timer.stop();
                    totalRealTime += timer.getRealTimeInSeconds();
                    timer.start();
                    totalNumEdges += ExpSetupHelper::doNHopEdgeQueries(graph, minTime, maxTime, timeExtent, nHops, nQueries, vertices);
                    timer.stop();
                    totalQueryTime += timer.getRealTimeInSeconds();
                    totalRealTime += timer.getRealTimeInSeconds();
                    totalQueryEdgeIO += graph.getEdgeReadIOCount();
                    totalQueryCount += nQueries;
                }
                double avgNumEdges = totalNumEdges / (double) totalQueryCount;
                double queryTime = totalQueryTime /totalQueryCount;
                double avgEdgeIOCount = totalQueryEdgeIO / (double) totalQueryCount;
                clog << "Size of nhop neighborhood query results for time extent " << timeExtent << " is " << avgNumEdges << endl;
                exp.addNewRecord();
                exp.addNewFieldValue("layout_mode", "Smart");
                exp.addNewFieldValue("candidate_count", k);
                exp.addNewFieldValue("initial_candidate_selection_policy", icsPolicyName);
                exp.addNewFieldValue("query_time_extent", timeExtent/1000.);
                exp.addNewFieldValue("query_run_time", queryTime);
                exp.addNewFieldValue("query_io_count", avgEdgeIOCount);
                exp.addNewFieldValue("query_result_size", avgNumEdges);
            }
        }
    }
    for (size_t l=0; l<Conf::LM_Smart; l++) {
        auto lMode = (Conf::LayoutMode) l;
        string lModeName = Conf::getLayoutModeName(lMode);
        stringstream name;
        name << gtname << "_variety";
        name << "_l_" << lModeName;
        string dbname = name.str();

        Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
        graphConf.layoutMode() = lMode;

        for (double timeExtent=minTimeExtentMsecs; timeExtent<=maxTimeExtentMsecs; timeExtent*=2) {
            vector<core::VertexId> vertices;
            {
                InteractionGraph<string,string> graph(graphConf);
                ExpSetupHelper::doVertexQueries(graph, minTime, maxTime, timeExtent, 1u, vertices);
            }
            AutoTimer timer;
            size_t totalNumEdges = 0;
            double totalQueryTime = 0.0;
            double totalRealTime = 0;
            size_t totalQueryEdgeIO = 0;
            size_t totalQueryCount = 0;
            while (totalRealTime<queryRunTime) {
                timer.start();
                system("purge");
                InteractionGraph<string,string> graph(graphConf);
                timer.stop();
                totalRealTime += timer.getRealTimeInSeconds();
                totalNumEdges += ExpSetupHelper::doNHopEdgeQueries(graph, minTime, maxTime, timeExtent, nHops, nQueries, vertices);
                timer.stop();
                totalQueryTime += timer.getRealTimeInSeconds();
                totalRealTime += timer.getRealTimeInSeconds();
                totalQueryEdgeIO += graph.getEdgeReadIOCount();
                totalQueryCount += nQueries;
            }
            double avgNumEdges = totalNumEdges / (double) totalQueryCount;
            double queryTime = totalQueryTime / totalQueryCount;
            double avgEdgeIOCount = totalQueryEdgeIO / (double) totalQueryCount;
            clog << "Size of nhop neighborhood query results for time extent " << timeExtent << " is " << avgNumEdges << endl;
            exp.addNewRecord();
            exp.addNewFieldValue("layout_mode", lModeName);
            exp.addNewFieldValue("candidate_count", (size_t)1);
            exp.addNewFieldValue("initial_candidate_selection_policy", "NA");
            exp.addNewFieldValue("query_time_extent", timeExtent/1000.);
            exp.addNewFieldValue("query_run_time", queryTime);
            exp.addNewFieldValue("query_io_count", avgEdgeIOCount);
            exp.addNewFieldValue("query_result_size", avgNumEdges);
        }
    }
    exp.close();
}

#endif
