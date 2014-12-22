#include <intergdb/core/InteractionGraph.h>

#include "gtest/gtest.h"  
#include <boost/filesystem.hpp>

#include <iostream>
#include <memory>

using namespace std;
using namespace intergdb::core;

class BasicReadWrite_test : public ::testing::Test 
{
public:
    BasicReadWrite_test() {}
protected:
    virtual void SetUp() 
    {
        conf.reset(new Conf("test", "/tmp/myigdb", 
            {{"v", DataType::STRING}}, 
            {{"a", DataType::STRING}, {"b", DataType::STRING}}));
        if (boost::filesystem::exists(conf->getStorageDir()))
            boost::filesystem::remove_all(conf->getStorageDir());
        boost::filesystem::create_directories(conf->getStorageDir());   
        graph.reset(new InteractionGraph(*conf));
    }
protected:
    std::unique_ptr<Conf> conf;
    std::unique_ptr<InteractionGraph> graph;
};

TEST_F(BasicReadWrite_test, WriteReadTest) 
{
    graph->createVertex(2, "v2");
    graph->createVertex(4, "v4");
    Timestamp ts = 7.0;
    graph->addEdge(2, 4, ts, "a-data", "b-data");
    graph->flush();

    graph.reset(NULL);
    sleep(1);
    graph.reset(new InteractionGraph(*conf));
    
    IntervalQuery q1(5.0, 10.0, {"a"});
    FocusedIntervalQuery q2(2, 5.0, 10.0, {"a"});

    InteractionGraph::VertexIterator iqIt = graph->processIntervalQuery(q1);
    EXPECT_EQ(iqIt.isValid(), true);
    EXPECT_EQ(iqIt.getVertexData()->getStringAttribute("v"), "v2");
    EXPECT_EQ(2, iqIt.getVertexId());
    iqIt.next();
    EXPECT_EQ(iqIt.isValid(), true);
    EXPECT_EQ(iqIt.getVertexData()->getStringAttribute("v"), "v4");
    EXPECT_EQ(4, iqIt.getVertexId());
    iqIt.next();
    EXPECT_EQ(iqIt.isValid(), false);

    InteractionGraph::EdgeIterator fiqIt = graph->processFocusedIntervalQuery(q2);
    EXPECT_EQ(fiqIt.isValid(), true);
    EXPECT_EQ(fiqIt.getEdgeData()->getStringAttribute("a"), "a-data");
    EXPECT_EQ(fiqIt.getEdgeData()->getStringAttribute("b"), "b-data");
    EXPECT_EQ(fiqIt.getToVertex(), 4);
    EXPECT_EQ(fiqIt.getTime(), 7);
    fiqIt.next();
    EXPECT_EQ(fiqIt.isValid(), false);
    
    FocusedIntervalQuery q3(2, 5.0, 10.0, {"b"});
    FocusedIntervalQuery q4(2, 5.0, 10.0, {"a"});
    graph->processFocusedIntervalQuery(q3);
    graph->processFocusedIntervalQuery(q4);
}

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}



