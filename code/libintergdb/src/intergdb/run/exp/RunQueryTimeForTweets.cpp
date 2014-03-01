#if false

#include <intergdb/run/exp/RunExp.h>
#include <intergdb/run/AutoTimer.h>
#include <intergdb/run/exp/Experiment.h>
#include <intergdb/run/exp/ExpSetupHelper.h>
#include <intergdb/core/Conf.h>
#include <intergdb/gen/InteractionGraphGenerator.h>

#include <tr1/functional>

using namespace std;
using namespace std::tr1;
using namespace intergdb::core;
using namespace intergdb::gen;
using namespace intergdb::run;
using namespace intergdb::run::exp;

void RunQueryTimeForTweets::process()
{
    using namespace std::tr1::placeholders;
    using std::tr1::placeholders::_1;
    using std::tr1::placeholders::_2;
    using std::tr1::placeholders::_3;
    using std::tr1::placeholders::_4;
    using std::tr1::placeholders::_5;
    using std::tr1::placeholders::_6;

    Experiment exp("tweets_and_query_time");
    exp.setDescription("Query time for tweet graph with different algs");
    exp.addFieldName("algorithm");
    exp.addFieldName("analytic");
    exp.addFieldName("query_time_extent");
    exp.addFieldName("query_io_count");
    exp.setKeepValues(false);
    exp.open();

    double minTime = 1368491654000 + 2 * 24 * 60 * 60 * 1000; // + 2 days
    double maxTime = minTime + 24 * 60 * 60 * 1000; // 24 hour / 1 say
    double minTimeExtentMsecs = 60 * 60 * 1000; // 1 hour
    double maxTimeExtentMsecs = 16 * minTimeExtentMsecs;

    size_t nQueries = 20;
    //double queryRunTime = 60.0;
    size_t nHops = 5;
    size_t nSteps = 20;
    size_t nIters = 1;

    string db_basename = "tweets";
    vector<Conf::LayoutMode> lmodes;
    lmodes.push_back(Conf::LM_Random);
    lmodes.push_back(Conf::LM_Smart);
    vector<VertexId> vertices;
    vector<function<void (InteractionGraph<int64_t,string> &, double, double, double)> > analytics;
    analytics.push_back(bind(&ExpSetupHelper::doBFSQueries<int64_t,string>,_1,_2,_3,_4, nHops, nQueries, ref(vertices)));
    analytics.push_back(bind(&ExpSetupHelper::doClusteringCoefficientQueries<int64_t,string>,_1,_2,_3,_4, nQueries, ref(vertices)));
    analytics.push_back(bind(&ExpSetupHelper::doRandomWalkQueries<int64_t,string>,_1,_2,_3,_4, nSteps, nQueries, ref(vertices)));
    analytics.push_back(bind(&ExpSetupHelper::doPageRankQueries<int64_t,string>,_1,_2,_3,_4, nIters));
    vector<string> analyticNames;
    analyticNames.push_back("nhops-5");
    analyticNames.push_back("ccoef");
    analyticNames.push_back("rwalk-20");
    analyticNames.push_back("prank-1");

    for (Conf::LayoutMode const & lmode : lmodes) {
        size_t analyticIndex = 0;
        string lmodeName = Conf::getLayoutModeName(lmode);
        for (auto const & analytic : analytics) {
            string dbname = db_basename+"_"+lmodeName;
            Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
            graphConf.layoutMode() = lmode;
            clog << "Starting " << analyticNames[analyticIndex] << endl;
            for (double timeExtent=minTimeExtentMsecs; timeExtent<=maxTimeExtentMsecs; timeExtent*=2) {
                vertices.clear();
                {
                    InteractionGraph<int64_t,string> graph(graphConf);
                    ExpSetupHelper::doVertexQueries(graph, minTime, maxTime, timeExtent, 1, vertices);
                }
                AutoTimer timer;
                size_t totalQueryEdgeIO = 0;
                InteractionGraph<int64_t,string> graph(graphConf);
                analytic(graph, minTime, maxTime, timeExtent);
                totalQueryEdgeIO += graph.getEdgeReadIOCount();
                double avgEdgeIOCount = totalQueryEdgeIO / (double) nQueries;
                clog << "Queries performed for time extent " << timeExtent << "." << endl;
                exp.addNewRecord();
                exp.addNewFieldValue("algorithm", lmodeName);
                exp.addNewFieldValue("analytic", analyticNames[analyticIndex]);
                exp.addNewFieldValue("query_time_extent", timeExtent/1000.);
                exp.addNewFieldValue("query_io_count", avgEdgeIOCount);
            }
            analyticIndex++;
        }
    }
    exp.close();
}

#endif
