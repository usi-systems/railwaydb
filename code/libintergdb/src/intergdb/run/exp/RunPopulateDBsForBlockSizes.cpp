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

void RunPopulateDBsForBlockSizes::process()
{
    Experiment exp("block_sizes_and_graph_stats");
    exp.setDescription("Statistics for graphs with different block sizes");
    exp.addFieldName("block_size");
    exp.addFieldName("locality");
    exp.addFieldName("db_size");
    exp.addFieldName("expected_size");
    exp.setKeepValues(false);
    exp.open();

    InteractionGraphGenerator gen;
    auto & genConf = gen.conf();
    ExpSetupHelper::setupGraphGeneratorWithDefaults(genConf);

    size_t vertexDataSize = 10;
    size_t edgeDataSize = 50;
    double duration = 12 * 60 * 60 * 1000; // 12 hour
    size_t blockSizeMin = 512;
    size_t blockSizeMax = 16384;

    auto graphType = InteractionGraphGenerator::GraphType::ScaleFree;
    string gtname = InteractionGraphGenerator::getGraphTypeName(graphType);
    genConf.graphType() = graphType;

    for (size_t blockSize=blockSizeMin; blockSize<=blockSizeMax; blockSize*=2) {
        gen.buildGraph();
        stringstream name;
        name << gtname << "_variety_blockSize_" << blockSize;
        string dbname = name.str();
        size_t expectedSize = 0;

        clog << "Cleaning old DBs..." << endl;
        ExpSetupHelper::clearDBFilesForTheExperiment(dbname);
        clog << "Cleaned old DBs." << endl;

        Conf graphConf = ExpSetupHelper::createGraphConfWithDefaults(dbname);
        graphConf.blockSize() = blockSize;

        clog << "Creating graph " << dbname << "..." << endl;
        InteractionGraph<string,string> graph(graphConf);
        clog << "Created graph " << dbname << "." << endl;
        clog << "Adding graph vertices..." << endl;
        size_t nVertices;
        ExpSetupHelper::addGraphVertices(gen, graph, vertexDataSize, &nVertices);
        expectedSize +=  (vertexDataSize + sizeof(VertexId) + sizeof(uint32_t)) * nVertices;
        clog << "Added graph vertices." << endl;
        clog << "Adding graph edges..." << endl;
        size_t nEdges;
        ExpSetupHelper::addGraphEdges(gen, graph, duration, edgeDataSize, &nEdges);
        expectedSize +=  (edgeDataSize + sizeof(VertexId)+sizeof(Timestamp)) * nEdges;
        clog << "Added graph edges." << endl;
        clog << "Flushing graph..." << endl;
        graph.flush();
        clog << "Flushed graph." << endl;

        exp.addNewRecord();
        exp.addNewFieldValue("block_size", blockSize);
        exp.addNewFieldValue("locality", sqrt(graph.getBlockStats().getAvgLocality()));
        string cmd = "du -k data/"+dbname+" | tail -1 | cut -f 1";
        string result = Helper::exec(cmd);
        result = result.substr(0, result.size()-1);
        exp.addNewFieldValue("db_size", result);
        exp.addNewFieldValue("expected_size", expectedSize/1024);
    }
    exp.close();
}

#endif
