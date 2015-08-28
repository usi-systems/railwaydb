#include "gtest/gtest.h"

#include <intergdb/core/InteractionGraph.h>

#include <cstdlib>
#include <iostream>

#include <boost/filesystem.hpp>

using namespace std;
using namespace intergdb::core;

class SchemaTest : public ::testing::Test
{
public:
    SchemaTest() {}

protected:
    virtual void SetUp()
    {
        auto storageDir = boost::filesystem::unique_path("/tmp/mydb_%%%%");
        conf.reset(new Conf("test", "/tmp/st_igdb",
                            {{"vertex-label", DataType::STRING}},
                            {{"a", DataType::INT64}, {"b", DataType::INT64}}));

        if (boost::filesystem::exists(conf->getStorageDir()))
            boost::filesystem::remove_all(conf->getStorageDir());
        boost::filesystem::create_directories(conf->getStorageDir());
        graph.reset(new InteractionGraph(*conf));
    }

    virtual void TearDown()
    {
        boost::filesystem::remove_all(graph->getConf().getStorageDir());
    }

protected:
    std::unique_ptr<Conf> conf;
    std::unique_ptr<InteractionGraph> graph;
};

TEST_F(SchemaTest, WriteReadTest)
{

    graph->createVertex(2, "v2");
    graph->createVertex(4, "v4");
    Timestamp ts = 7.0;

    EXPECT_NO_THROW(graph->addEdge(2, 4, ts, INT64_C(1), INT64_C(2)));

    EXPECT_ANY_THROW(graph->addEdge(2, 4, ts+1, INT64_C(1), INT64_C(2), INT64_C(3)));
/*
    caughtError = false;
    try {
        graph->addEdge(2, 4, ts, "1", "2");
    } catch(runtime_error const & e) {
        caughtError = true;
    }
    if (!caughtError) {
        cout << "test FAILED: did not catch wrong schema type.\n" << endl;
        return EXIT_FAILURE;
    }
    graph->flush();
*/

}



