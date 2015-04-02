#include "gtest/gtest.h"

#include <intergdb/core/InteractionGraph.h>

#include <cstdlib>
#include <iostream>
#include <climits>
#include <set>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace intergdb::core;

using boost::lexical_cast;

#define MAX_WT numeric_limits<double>::max()

class SpanningTreeTest : public ::testing::Test
{
public:
    SpanningTreeTest() {}

protected:
    virtual void SetUp()
    {
        auto storageDir = boost::filesystem::unique_path("/tmp/mydb_%%%%");
        conf.reset(new Conf("test", storageDir.string(),
            {{"vertex-label", DataType::STRING}},
            {{"weight", DataType::DOUBLE}}));
        if (boost::filesystem::exists(conf->getStorageDir()))
            boost::filesystem::remove_all(conf->getStorageDir());
        boost::filesystem::create_directories(conf->getStorageDir());
        graph.reset(new InteractionGraph(*conf));
    }

    virtual void TearDown()
    {
        boost::filesystem::remove_all(graph->getConf().getStorageDir());
    }

    void msp(InteractionGraph &graph, VertexId q0,
             set<pair<VertexId,VertexId> > &tree,
             double startTime, double endTime,
             vector<string> const & attributes)
    {
        set<VertexId> out; // The set of vertices not in the tree
        set<VertexId> in;  // The set of vertices in the tree
        VertexId v{};
        VertexId u{};

        int i = 0;
        IntervalQuery q1(startTime, endTime);
        for (auto iqIt = graph.processIntervalQuery(q1); iqIt.isValid(); iqIt.next()) {
            if (i == 0) {
                v = iqIt.getVertexId();
            }
            out.insert(iqIt.getVertexId());
            i++;
        }


        double minWeight = MAX_WT;
        double edgeWeight = MAX_WT;
        VertexId minDst = v;
        VertexId minSrc = v;

        while (!out.empty()) {
            in.insert(minDst);       // add v to the tree.
            out.erase(minDst);       // remove v from "not in the tree"
            minWeight = MAX_WT;
            for (auto it=in.begin(); it!=in.end(); ++it) {
                u = *it;
                FocusedIntervalQuery fiq(u, startTime, endTime, attributes);
                for (auto fiqIt = graph.processFocusedIntervalQuery(fiq); fiqIt.isValid(); fiqIt.next()) {
                    v = fiqIt.getToVertex();
                    if (in.find(v) == in.end()) {
                        edgeWeight = fiqIt.getEdgeData()->getDoubleAttribute("weight");
                        if (edgeWeight <= minWeight) {
                            minWeight = edgeWeight;
                            minDst = v;
                            minSrc = u;
                        }
                    }
                }
            }
            tree.insert(make_pair (minSrc,minDst));
        }
    }

protected:
    unique_ptr<Conf> conf;
    unique_ptr<InteractionGraph> graph;
};


TEST_F(SpanningTreeTest, WriteReadTest)
{
    graph->createVertex(0, "v0");
    graph->createVertex(1, "v1");
    graph->createVertex(2, "v2");
    graph->createVertex(3, "v3");
    graph->createVertex(4, "v4");
    graph->createVertex(5, "v5");
    graph->createVertex(6, "v6");
    graph->createVertex(7, "v7");
    Timestamp ts = 1.0;
    graph->addEdge(3, 5, ts++, 1.);
    graph->addEdge(0, 2, ts++, 2.);
    graph->addEdge(0, 7, ts++, 3.);
    graph->addEdge(0, 1, ts++, 4.);
    graph->addEdge(3, 4, ts++, 5.);
    graph->addEdge(5, 4, ts++, 6.);
    graph->addEdge(0, 6, ts++, 7.);
    graph->addEdge(7, 1, ts++, 8.);
    graph->addEdge(7, 6, ts++, 9.);
    graph->addEdge(7, 4, ts++, 10.);
    graph->addEdge(6, 4, ts++, 11.);
    graph->addEdge(0, 5, ts++, 12.);
    graph->flush();

    double startTime = 0.0, endTime = ts;
    vector<string> attributes = {"weight"};
    set<pair<VertexId,VertexId> > tree;

    msp(*graph, 0, tree, startTime, endTime, attributes);

    auto it = tree.find(make_pair(0, 1));
    EXPECT_NE(it, tree.end());

    it = tree.find(make_pair(0, 6));
    EXPECT_NE(it, tree.end());

    it = tree.find(make_pair(0, 7));
    EXPECT_NE(it, tree.end());

    it = tree.find(make_pair(2, 0));
    EXPECT_NE(it, tree.end());

    it = tree.find(make_pair(3, 5));
    EXPECT_NE(it, tree.end());

    it = tree.find(make_pair(4, 3));
    EXPECT_NE(it, tree.end());

    it = tree.find(make_pair(7, 4));
    EXPECT_NE(it, tree.end());
}
