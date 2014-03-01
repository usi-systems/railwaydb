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

void RunPopulateDBsForExpirationHeapSizes::process()
{
    Experiment exp("expiration_heap_sizes_and_graph_stats");
    exp.setDescription("Statistics for graphs with different expiration heap sizes");
    exp.addFieldName("expiration_heap_size_ratio");
    exp.addFieldName("locality");
    exp.setKeepValues(false);
    exp.open();

    InteractionGraphGenerator gen;
    auto & genConf = gen.conf();
    ExpSetupHelper::setupGraphGeneratorWithDefaults(genConf);

    size_t vertexDataSize = 10;
    size_t edgeDataSize = 50;
    double duration = 12 * 60 * 60 * 1000; // 12 hour
    double heapSizeRatioMin = 0.00125;
    double heapSizeRatioMax = 0.201;

    auto graphType = InteractionGraphGenerator::GraphType::ScaleFree;
    string gtname = InteractionGraphGenerator::getGraphTypeName(graphType);
    genConf.graphType() = graphType;

    for (double heapSizeRatio=heapSizeRatioMin; heapSizeRatio<=heapSizeRatioMax; heapSizeRatio*=2) {
        gen.buildGraph();
        stringstream name;
        name << gtname << "_variety_expirationHeapSizeRatio_" << heapSizeRatio;
        string dbname = name.str();

        clog << "Cleaning old DBs..." << endl;
        ExpSetupHelper::clearDBFilesForTheExperiment(dbname);
        clog << "Cleaned old DBs." << endl;

        Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
        graphConf.expirationMapSize() = heapSizeRatio * graphConf.windowSize();

        clog << "Creating graph " << dbname << "..." << endl;
        InteractionGraph<string,string> graph(graphConf);
        clog << "Created graph " << dbname << "." << endl;
        clog << "Adding graph vertices..." << endl;
        ExpSetupHelper::addGraphVertices(gen, graph, vertexDataSize);
        clog << "Added graph vertices." << endl;
        clog << "Adding graph edges..." << endl;
        ExpSetupHelper::addGraphEdges(gen, graph, duration, edgeDataSize);
        clog << "Added graph edges." << endl;
        clog << "Flushing graph..." << endl;
        graph.flush();
        clog << "Flushed graph." << endl;

        exp.addNewRecord();
        exp.addNewFieldValue("expiration_heap_size_ratio", heapSizeRatio);
        exp.addNewFieldValue("locality", sqrt(graph.getBlockStats().getAvgLocality()));
    }
    exp.close();
}

#endif
