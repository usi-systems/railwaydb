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

void RunQueryTimeForHops::process()
{
    Experiment exp("hops_and_query_time");
    exp.setDescription("Query time for graphs with different nhops");
    exp.addFieldName("nhops");
    exp.addFieldName("algorithm");
    exp.addFieldName("query_time_extent");
    exp.addFieldName("query_run_time");
    exp.addFieldName("query_io_count");
    exp.addFieldName("query_result_size");
    exp.setKeepValues(false);
    exp.open();

    double minTime = 0;
    double maxTime = 12 * 60 * 60 * 1000; // 12 hour
    double minTimeExtentMsecs = 16 * 1000; // 16 second
    double maxTimeExtentMsecs = 60 * 60 * 1000; // 1 hour
    size_t nQueries = 10; // make sure numQueriesToRun*maxTimeExtent < maxTime - minTime
    double queryRunTime = 60.0;

    auto graphType = InteractionGraphGenerator::GraphType::ScaleFree;
    string gtname = InteractionGraphGenerator::getGraphTypeName(graphType);

    size_t k = 16;
    vector<Conf::LayoutMode> lmodes;
    lmodes.push_back(Conf::LM_Random);
    lmodes.push_back(Conf::LM_Smart);
    for (Conf::LayoutMode const & lmode : lmodes) {
        for (size_t nHops=1; nHops<=6; nHops++) {
            auto icsPolicy = Conf::SmartLayoutConf::ICS_Old;
            string icsPolicyName = Conf::SmartLayoutConf::getInitialCandidateSelectionPolicyName(icsPolicy);
            string lModeName = Conf::getLayoutModeName(lmode);
            stringstream name;
            name << gtname << "_variety";
            if (lmode==Conf::LM_Smart) {
                name << "_l_Smart_k_" << k;
                name << "_p_" << icsPolicyName;
            } else {
                name << "_l_" << lModeName;
            }
            string dbname = name.str();

            Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
            graphConf.layoutMode() = lmode;
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
                exp.addNewFieldValue("nhops", nHops);
                exp.addNewFieldValue("algorithm", lModeName);
                exp.addNewFieldValue("query_time_extent", timeExtent/1000.);
                exp.addNewFieldValue("query_run_time", queryTime);
                exp.addNewFieldValue("query_io_count", avgEdgeIOCount);
                exp.addNewFieldValue("query_result_size", avgNumEdges);
            }
        }
    }
    exp.close();
}

#endif
