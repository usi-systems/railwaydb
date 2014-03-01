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

void RunPopulateDBsForCandidates::process()
{
    Experiment exp("candidates_and_graph_stats");
    exp.setDescription("Statistics for graphs with different candidate sizes");
    exp.addFieldName("algorithm");
    exp.addFieldName("candidate_count");
    exp.addFieldName("locality");
    exp.addFieldName("num_edges");
    exp.addFieldName("time");
    exp.setKeepValues(false);
    exp.open();

    InteractionGraphGenerator gen;
    auto & genConf = gen.conf();
    ExpSetupHelper::setupGraphGeneratorWithDefaults(genConf);

    size_t vertexDataSize = 10;
    size_t edgeDataSize = 50;
    double duration = 12 * 60 * 60 * 1000; // 12 hour

    auto graphType = InteractionGraphGenerator::GraphType::ScaleFree;
    string gtname = InteractionGraphGenerator::getGraphTypeName(graphType);
    genConf.graphType() = graphType;

    for (size_t k=2; k<=30; k++) {
        gen.buildGraph();
        stringstream name;
        name << gtname << "_variety";
        name << "_l_Smart_k_" << k;
        string dbname = name.str();
        clog << "Cleaning old DBs..." << endl;
        ExpSetupHelper::clearDBFilesForTheExperiment(dbname);
        clog << "Cleaned old DBs." << endl;

        Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
        graphConf.layoutMode() = Conf::LM_Smart;
        graphConf.smartLayoutConf().initialCandidateCount() = k;

        clog << "Creating graph " << dbname << "..." << endl;
        InteractionGraph<string,string> graph(graphConf);
        clog << "Created graph " << dbname << "." << endl;
        clog << "Adding graph vertices..." << endl;
        ExpSetupHelper::addGraphVertices(gen, graph, vertexDataSize);
        clog << "Added graph vertices." << endl;

        AutoTimer timer;
        timer.start();
        clog << "Adding graph edges..." << endl;
        size_t numEdges;
        ExpSetupHelper::addGraphEdges(gen, graph, duration, edgeDataSize, &numEdges);
        clog << "Added graph edges." << endl;

        clog << "Flushing graph..." << endl;
        graph.flush();
        clog << "Flushed graph." << endl;
        timer.stop();
        double time = timer.getRealTimeInSeconds();

        exp.addNewRecord();
        exp.addNewFieldValue("algorithm", "Greedy");
        exp.addNewFieldValue("candidate_count", k);
        exp.addNewFieldValue("locality", sqrt(graph.getBlockStats().getAvgLocality()));
        exp.addNewFieldValue("time", time);
        exp.addNewFieldValue("num_edges", numEdges);
    }
    {
        gen.buildGraph();
        stringstream name;
        name << gtname << "_variety";
        name << "_l_Random_k_" << 1;
        string dbname = name.str();
        clog << "Cleaning old DBs..." << endl;
        ExpSetupHelper::clearDBFilesForTheExperiment(dbname);
        clog << "Cleaned old DBs." << endl;

        Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
        graphConf.layoutMode() = Conf::LM_Random;

        clog << "Creating graph " << dbname << "..." << endl;
        InteractionGraph<string,string> graph(graphConf);
        clog << "Created graph " << dbname << "." << endl;
        clog << "Adding graph vertices..." << endl;
        ExpSetupHelper::addGraphVertices(gen, graph, vertexDataSize);
        clog << "Added graph vertices." << endl;

        AutoTimer timer;
        timer.start();
        clog << "Adding graph edges..." << endl;
        size_t numEdges;
        ExpSetupHelper::addGraphEdges(gen, graph, duration, edgeDataSize, &numEdges);
        clog << "Added graph edges." << endl;

        clog << "Flushing graph..." << endl;
        graph.flush();
        clog << "Flushed graph." << endl;
        timer.stop();
        double time = timer.getRealTimeInSeconds();

        exp.addNewRecord();
        exp.addNewFieldValue("algorithm", "Random");
        exp.addNewFieldValue("candidate_count", static_cast<size_t>(1));
        exp.addNewFieldValue("locality", sqrt(graph.getBlockStats().getAvgLocality()));
        exp.addNewFieldValue("time", time);
        exp.addNewFieldValue("num_edges", numEdges);
    }
    exp.close();
}

#endif
