#if false

#include <intergdb/run/exp/RunExp.h>
#include <intergdb/run/AutoTimer.h>
#include <intergdb/run/exp/Experiment.h>
#include <intergdb/run/exp/ExpSetupHelper.h>
#include <intergdb/core/Conf.h>
#include <intergdb/core/Helper.h>
#include <intergdb/gen/InteractionGraphGenerator.h>

using namespace std;
using namespace intergdb::core;
using namespace intergdb::gen;
using namespace intergdb::run;
using namespace intergdb::run::exp;

void RunPopulateDBsForSkews::process()
{
    Experiment exp("skews_and_graph_stats");
    exp.setDescription("Statistics for graphs with different block sizes");
    exp.addFieldName("skew");
    exp.addFieldName("algorithm");
    exp.addFieldName("locality");
    exp.setKeepValues(false);
    exp.open();

    InteractionGraphGenerator gen;
    auto & genConf = gen.conf();
    ExpSetupHelper::setupGraphGeneratorWithDefaults(genConf);

    size_t vertexDataSize = 10;
    size_t edgeDataSize = 50;
    double duration = 12 * 60 * 60 * 1000; // 12 hour
    double skewMin = 0.5;
    double skewMax = 3.10;
    double skewDelta = 0.5;

    auto graphType = InteractionGraphGenerator::GraphType::ScaleFree;
    string gtname = InteractionGraphGenerator::getGraphTypeName(graphType);
    genConf.graphType() = graphType;

    vector<Conf::LayoutMode> lmodes;
    lmodes.push_back(Conf::LM_Random);
    lmodes.push_back(Conf::LM_Smart);
    for (Conf::LayoutMode const & lmode : lmodes) {
        for (double skew=skewMin; skew<=skewMax; skew+=skewDelta) {
            genConf.popularityZipfParam() = skew;
            gen.buildGraph();
            stringstream name;
            name << gtname << "_variety_skew_" << skew;
            name << "_layout_" << Conf::getLayoutModeName(lmode);
            string dbname = name.str();

            clog << "Cleaning old DBs..." << endl;
            ExpSetupHelper::clearDBFilesForTheExperiment(dbname);
            clog << "Cleaned old DBs." << endl;

            Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
            graphConf.layoutMode() = lmode;

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
            exp.addNewFieldValue("skew", skew);
            exp.addNewFieldValue("algorithm", Conf::getLayoutModeName(lmode));
            exp.addNewFieldValue("locality", sqrt(graph.getBlockStats().getAvgLocality()));
        }
    }
    exp.close();
}

#endif
