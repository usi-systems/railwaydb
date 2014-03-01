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

void RunQueryTimeForBlockSizes::process()
{
    Experiment exp("block_sizes_and_query_time");
    exp.setDescription("Query time for graphs with different block sizes");
    exp.addFieldName("block_size");
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
    size_t blockSizeMin = 512;
    size_t blockSizeMax = 16384;

    auto graphType = InteractionGraphGenerator::GraphType::ScaleFree;
    string gtname = InteractionGraphGenerator::getGraphTypeName(graphType);

    for (size_t blockSize=blockSizeMin; blockSize<=blockSizeMax; blockSize*=2) {
        stringstream name;
        name << gtname << "_variety_blockSize_" << blockSize;
        string dbname = name.str();

        Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
        graphConf.blockSize() = blockSize;

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
            exp.addNewFieldValue("block_size", blockSize);
            exp.addNewFieldValue("query_time_extent", timeExtent/1000.);
            exp.addNewFieldValue("query_run_time", queryTime);
            exp.addNewFieldValue("query_io_count", avgEdgeIOCount);
            exp.addNewFieldValue("query_result_size", avgNumEdges);
        }
    }

    exp.close();
}

#endif
