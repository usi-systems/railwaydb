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

void RunPopulateDBsForDifferentAlgs::process()
{
    Experiment exp("algorithms_and_graph_stats");
    exp.setDescription("Statistics for graphs with different algs");
    exp.addFieldName("layout_mode");
    exp.addFieldName("candidate_count");
    exp.addFieldName("initial_candidate_selection_policy");
    exp.addFieldName("locality");
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

    for (size_t k=1; k<=36; k*=2) {
        for (size_t p=0; p<Conf::SmartLayoutConf::ICS_nPolicies; ++p) {
            gen.buildGraph();
            auto icsPolicy = (Conf::SmartLayoutConf::InitialCandidateSelectionPolicy) p;
            string icsPolicyName = Conf::SmartLayoutConf::getInitialCandidateSelectionPolicyName(icsPolicy);
            stringstream name;
            name << gtname << "_variety";
            name << "_l_Smart_k_" << k;
            name << "_p_" << icsPolicyName;
            string dbname = name.str();
            clog << "Cleaning old DBs..." << endl;
            ExpSetupHelper::clearDBFilesForTheExperiment(dbname);
            clog << "Cleaned old DBs." << endl;

            Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
            graphConf.layoutMode() = Conf::LM_Smart;
            graphConf.smartLayoutConf().initialCandidateCount() = k;
            graphConf.smartLayoutConf().initialCandidateSelectionPolicy() = icsPolicy;

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
            exp.addNewFieldValue("layout_mode", "Smart");
            exp.addNewFieldValue("candidate_count", k);
            exp.addNewFieldValue("initial_candidate_selection_policy", icsPolicyName);
            exp.addNewFieldValue("locality", sqrt(graph.getBlockStats().getAvgLocality()));
        }
    }
    for (size_t l=0; l<Conf::LM_Smart; l++) {
        gen.buildGraph();
        auto lMode = (Conf::LayoutMode) l;
        string lModeName = Conf::getLayoutModeName(lMode);
        stringstream name;
        name << gtname << "_variety";
        name << "_l_" << lModeName;
        string dbname = name.str();
        clog << "Cleaning old DBs..." << endl;
        ExpSetupHelper::clearDBFilesForTheExperiment(dbname);
        clog << "Cleaned old DBs." << endl;

        Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
        graphConf.layoutMode() = lMode;

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
        exp.addNewFieldValue("layout_mode", lModeName);
        exp.addNewFieldValue("candidate_count", (size_t)1);
        exp.addNewFieldValue("initial_candidate_selection_policy", "NA");
        exp.addNewFieldValue("locality", sqrt(graph.getBlockStats().getAvgLocality()));
    }
    exp.close();
}

#endif
