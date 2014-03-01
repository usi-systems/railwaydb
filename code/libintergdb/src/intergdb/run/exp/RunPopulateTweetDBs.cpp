#if false

#include <intergdb/run/exp/RunExp.h>
#include <intergdb/run/AutoTimer.h>
#include <intergdb/run/exp/Experiment.h>
#include <intergdb/run/exp/ExpSetupHelper.h>

using namespace std;
using namespace intergdb::core;
using namespace intergdb::gen;
using namespace intergdb::run;
using namespace intergdb::run::exp;

void RunPopulateTweetDBs::process()
{
    string db_basename = "tweets";
    Experiment exp("tweets_graph_stats");
    exp.setDescription("Statistics for the twitter graph");
    exp.addFieldName("locality");
    exp.addFieldName("num_vertices");
    exp.addFieldName("num_edges");
    exp.setKeepValues(false);
    exp.open();
    {
        string dbname = db_basename + "_Smart";
        ExpSetupHelper::clearDBFilesForTheExperiment(dbname);
        Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
        InteractionGraph<int64_t,string> graph(graphConf);
        size_t numVertices, numEdges;
        ExpSetupHelper::populateGraphFromTweets("reduced_tweets2_sorted", graph, &numVertices, &numEdges);
        exp.addNewRecord();
        exp.addNewFieldValue("locality", sqrt(graph.getBlockStats().getAvgLocality()));
        exp.addNewFieldValue("num_vertices", numVertices);
        exp.addNewFieldValue("num_edges", numEdges);
    }
    {
        string dbname = db_basename + "_Random";
        ExpSetupHelper::clearDBFilesForTheExperiment(dbname);
        Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
        graphConf.layoutMode() = Conf::LM_Random;
        InteractionGraph<int64_t,string> graph(graphConf);
        size_t numVertices, numEdges;
        ExpSetupHelper::populateGraphFromTweets("reduced_tweets2_sorted", graph, &numVertices, &numEdges);
        exp.addNewRecord();
        exp.addNewFieldValue("locality", sqrt(graph.getBlockStats().getAvgLocality()));
        exp.addNewFieldValue("num_vertices", numVertices);
        exp.addNewFieldValue("num_edges", numEdges);
    }
    exp.close();
    /*
    size_t vertexDataSize = 10;
    size_t edgeDataSize = 50;
    double duration = 7 * 24 * 60 * 60 * 1000; // 1 week

    for (size_t i=0; i<InteractionGraphGenerator::numGraphTypes; ++i) {
        auto graphType = (InteractionGraphGenerator::GraphType) i;
        genConf.graphType() = graphType;
        string gtname = InteractionGraphGenerator::getGraphTypeName(graphType);
        gen.buildGraph();
        clog << "Cleaning old DBs..." << endl;
        ExpSetupHelper::clearDBFilesForTheExperiment(gtname);
        clog << "Cleaned old DBs." << endl;

        Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(gtname);
        InteractionGraph<string,string> graph(graphConf);
        clog << "Adding graph vertices..." << endl;
        ExpSetupHelper::addGraphVertices(gen, graph, vertexDataSize);
        clog << "Added graph vertices." << endl;

        clog << "Adding graph edges..." << endl;
        ExpSetupHelper::addGraphEdges(gen, graph, duration, edgeDataSize);
        clog << "Added graph edges." << endl;

        clog << "Flushing graph..." << endl;
        graph.flush();
        clog << "Flushed graph." << endl;
    }
    */
}

#endif
