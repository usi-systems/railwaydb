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

void RunPopulateDBs::process()
{
    InteractionGraphGenerator gen;
    auto & genConf = gen.conf();
    ExpSetupHelper::setupGraphGeneratorWithDefaults(genConf);

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
}

#endif

