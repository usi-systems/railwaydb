#include <intergdb/run/test/RunTest.h>

#include <intergdb/core/InteractionGraph.h>
#include <intergdb/gen/InteractionGraphGenerator.h>

#include <boost/filesystem.hpp>

#include <iostream>
#include <stdexcept>
#include <unordered_set>

using namespace std;
using namespace intergdb::gen;
using namespace intergdb::run;
using namespace intergdb::run::test;
using namespace intergdb::core;

void RunInteractionGraphTest::process()
{
    InteractionGraphGenerator gen;
    cout << "Building generative graph..." << endl;
    {
        auto & conf = gen.conf();
        conf.popularityGroupCount() = 10000;
        conf.popularityZipfParam() = 1.5;
        conf.graphType() = InteractionGraphGenerator::ScaleFree;
        conf.numVertices() = 100*1000;
        conf.numEdges() = 10*conf.numVertices();
        conf.interArrivalTimeMean() = InteractionGraphGenerator::Conf::
            convertPerDayCountToInterArrivalDelayInMillisecs(1000*conf.numVertices());
        gen.buildGraph();
    }
    cout << "Built generative graph." << endl;

    cout << "Cleaning test dir..." << endl;
    boost::filesystem::path testDir("/tmp/"+getClassName());
    {
        boost::filesystem::remove_all(testDir);
        boost::filesystem::create_directory(testDir);
    }
    cout << "Cleaned test dir." << endl;

    Conf conf("test", testDir.string());
    cout << "Creating interaction graph configuration..." << endl;
    {
        conf.blockSize() = 1024; // num bytes
        conf.windowSize() = 10*1000; // num edges
        conf.blockBufferSize() = 100; // num blocks
        conf.vertexDataBufferSize() = 100*1000; // num verices
        conf.expirationMapSize() = 1000; // num edges
    }
    cout << "Created interaction graph configuration." << endl;

    cout << "Creating interaction graph..." << endl;
    typedef InteractionGraph<std::string,std::string> Graph;
    Graph graph(conf);
    cout << "Created interaction graph." << endl;

    cout << "Adding vertices to the graph..." << endl;
    {
        for (auto vit = gen.getVertexIterator();
                vit.isValid(); vit.next()) {
            VertexId id = vit.getVertex();
            graph.createVertex(id, string("abc"));
        }
    }
    cout << "Added vertices to the graph." << endl;

    size_t numEffectiveVertices = 0;
    Timestamp oldestTime, newestTime;
    cout << "Adding edges..." << endl;
    {
        unordered_set<VertexId> seenVertices;
        for (size_t i=0; i<100*1000; ++i) {
            auto edge = gen.generateNextEdge();

            //cerr << edge.getFromVertex() << "-" << edge.getToVertex() << " " << edge.getTime() << endl;
            graph.addEdge(edge.getFromVertex(), edge.getToVertex(),
                          "def", edge.getTime());
            if (i==0)
                oldestTime = edge.getTime();
            newestTime = edge.getTime();

            seenVertices.insert(edge.getFromVertex());
            seenVertices.insert(edge.getToVertex());

            if ((i+1)%10000==0)
                cout << "Added " << (i+1) << " edges" << endl;
        }
        BlockStats const & stats = graph.getBlockStats();
        cout << "\tavg. size (bytes) of blocks = " << stats.getAvgSize() << endl;
        cout << "\tavg. # of edges in blocks = " << stats.getAvgNumEdges() << endl;
        cout << "\tavg. # of nlists in blocks = " << stats.getAvgNumNLists() << endl;
        cout << "\tavg. nlist size in blocks = " << stats.getAvgNListSize() << endl;
        cout << "\tavg. locality of blocks = " << stats.getAvgLocality() << endl;
        numEffectiveVertices = seenVertices.size();
    }
    cout << "Added edges." << endl;

    cout << "Flushing graph..." << endl;
    graph.flush();
    cout << "Flushed graph." << endl;

    cout << "Searching for vertices using different time ranges..." << endl;
    {
        Timestamp midTime = (newestTime + oldestTime) / 2.0;
        cerr << "Time range: (" << oldestTime << "," << midTime << "," << newestTime << ")" << endl;
        double timeDiff = newestTime - oldestTime;
        size_t nBuckets = 10192;
        size_t resSize = 0;
        for (size_t i=1; i<=nBuckets; i*=2) {
            double halfRange = 0.5 * timeDiff * (((double)i)/nBuckets);
            Timestamp startTime = (midTime>halfRange) ? (midTime-halfRange) : 0;
            Timestamp endTime = midTime + halfRange;
            cerr << "Query range: " << startTime << " to " << endTime;
            Graph::VertexIterator iqIt = graph.processIntervalQuery(startTime, endTime);
            for (resSize = 0; iqIt.isValid(); iqIt.next(), resSize++) {
                VertexId id = iqIt.getVertexId();
                string data = *iqIt.getVertexData();
                //cerr << id << " " << data << endl;
                (void) id;
            }
            cerr << ", num results: " << resSize << endl;
        }
        cerr << "Total effective vertices: " << numEffectiveVertices << endl;
        Graph::VertexIterator iqIt = graph.processIntervalQuery(oldestTime, newestTime);
        for (resSize = 0; iqIt.isValid(); iqIt.next(), resSize++);
        assert(resSize == numEffectiveVertices);
        iqIt = graph.processIntervalQuery(midTime, midTime);
        for (resSize = 0; iqIt.isValid(); iqIt.next(), resSize++);
        assert(resSize == 0);
    }
    cout << "Searched for vertices using different time ranges." << endl;

    cout << "Searching for edges using different time ranges..." << endl;
    {
        Timestamp midTime = (newestTime + oldestTime) / 2.0;
        cerr << "Time range: (" << oldestTime << "," << midTime << "," << newestTime << ")" << endl;
        double timeDiff = newestTime - oldestTime;
        size_t nBuckets = 10192;
        for (size_t i=1; i<=nBuckets; i*=2) {
            double halfRange = 0.5 * timeDiff * (((double)i)/nBuckets);
            Timestamp startTime = (midTime>halfRange) ? (midTime-halfRange) : 0;
            Timestamp endTime = midTime + halfRange;
            cerr << "Query range: " << startTime << " to " << endTime;
            Graph::EdgeIterator fiqIt = graph.processFocusedIntervalQuery(gen.getRandomPopularVertex(), startTime, endTime);
            size_t resSize = 0;
            for (; fiqIt.isValid(); fiqIt.next(), resSize++) {
                string data = *fiqIt.getEdgeData();
                VertexId to = fiqIt.getToVertex();
                Timestamp tm  = fiqIt.getTime();
                //cerr << to << " " << tm << " " << data << endl;
                (void) to; (void) tm;
            }
            cerr << ", num results: " << resSize << endl;
        }

    }
    cout << "Searched for edges using different time ranges." << endl;
}


