#if false

#include <intergdb/run/exp/RunExp.h>
#include <intergdb/run/AutoTimer.h>
#include <intergdb/run/exp/Experiment.h>
#include <intergdb/run/exp/ExpSetupHelper.h>
#include <intergdb/gen/InteractionGraphGenerator.h>

using namespace std;
using namespace intergdb::core;
using namespace intergdb::gen;
using namespace intergdb::run;
using namespace intergdb::run::exp;

void RunBasicTimeRange::process()
{
    Experiment exp("graph_type_query_range_query_time");
    exp.setDescription("Query time as a function of graph type and query range");
    exp.addFieldName("query_time_extent");
    exp.addFieldName("graph_type");
    exp.addFieldName("vertex_query_time");
    exp.addFieldName("vertex_query_result_size");
    exp.addFieldName("edge_query_time");
    exp.addFieldName("edge_query_result_size");
    exp.setKeepValues(false);
    exp.open();

    size_t numQueriesToRun = 10; // make sure numQueriesToRun*maxTimeExtent < maxTime - minTime
    double minTime = 0.0;
    double maxTime = 7 * 24 * 60 * 60 * 1000; // 7 days
    double minTimeExtentMsecs = 16 * 1000; // 16 second
    double maxTimeExtentMsecs = 4 * 60 * 60 * 1000; // 4 hours
    double queryRunTime = 60.0;

    for (size_t i=0; i<InteractionGraphGenerator::numGraphTypes; ++i) {
        auto graphType = (InteractionGraphGenerator::GraphType) i;
        if (graphType==InteractionGraphGenerator::GraphType::ErdosReyni)
            continue;
        string gtname = InteractionGraphGenerator::getGraphTypeName(graphType);
        Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(gtname);

        //vector<core::VertexId> vertices;
        //size_t numVertices = ExpSetupHelper::doVertexQueries(graph, minTime, maxTime, 16384 * 1000, 3, vertices);
        //return;

        for (double timeExtent=minTimeExtentMsecs; timeExtent<=maxTimeExtentMsecs; timeExtent*=2) {
            exp.addNewRecord();
            exp.addNewFieldValue("query_time_extent", timeExtent/1000.0);
            exp.addNewFieldValue("graph_type", InteractionGraphGenerator::getGraphTypeName(graphType));
            vector<core::VertexId> vertices;
            AutoTimer timer;
            {
                size_t totalNumVertices = 0;
                double totalRealTime = 0;
                double totalQueryTime = 0.0;
                size_t totalQueryCount = 0;
                while (totalRealTime<queryRunTime) {
                    timer.start();
                    InteractionGraph<string,string> graph(graphConf);
                    system("purge");
                    timer.stop();
                    totalRealTime += timer.getRealTimeInSeconds();
                    timer.start();
                    vertices.clear();
                    totalNumVertices += ExpSetupHelper::doVertexQueries(graph, minTime, maxTime, timeExtent, numQueriesToRun, vertices);
                    timer.stop();
                    totalQueryTime += timer.getRealTimeInSeconds();
                    totalRealTime += timer.getRealTimeInSeconds();
                    totalQueryCount += numQueriesToRun;
                }
                double avgNumVertices = totalNumVertices / (double) totalQueryCount;
                double queryTime = totalQueryTime / totalQueryCount;
                clog << "Avg. size of vertex query results for time extent " << timeExtent << " is " << avgNumVertices << endl;
                exp.addNewFieldValue("vertex_query_time", queryTime);
                exp.addNewFieldValue("vertex_query_result_size", avgNumVertices);
            }

            {
                size_t totalNumEdges = 0;
                double totalRealTime = 0;
                double totalQueryTime = 0.0;
                size_t totalQueryCount = 0;
                while (totalRealTime<queryRunTime) {
                    timer.start();
                    InteractionGraph<string,string> graph(graphConf);
                    system("purge");
                    timer.stop();
                    totalRealTime += timer.getRealTimeInSeconds();
                    timer.start();
                    totalNumEdges += ExpSetupHelper::doEdgeQueries(graph, minTime, maxTime, timeExtent, numQueriesToRun, vertices);
                    timer.stop();
                    totalQueryTime += timer.getRealTimeInSeconds();
                    totalRealTime += timer.getRealTimeInSeconds();
                    totalQueryCount += numQueriesToRun;
                }
                double avgNumEdges = totalNumEdges / (double) totalQueryCount;
                double queryTime = totalQueryTime /totalQueryCount;
                clog << "Avg. size of neighborhood query results for time extent " << timeExtent << " is " << avgNumEdges << endl;
                exp.addNewFieldValue("edge_query_time", queryTime);
                exp.addNewFieldValue("edge_query_result_size", avgNumEdges);
            }
            clog << endl;
        }
    }
    exp.close();
}

#endif
