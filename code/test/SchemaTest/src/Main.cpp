#include <intergdb/core/InteractionGraph.h>
#include <intergdb/core/Schema.h>

#include <cstdlib>
#include <iostream>


#include <boost/filesystem.hpp>

using namespace std;
using namespace intergdb::core;

int main()
{
    Conf conf("test", "/tmp/st_igdb", {{"vertex-label",Attribute::STRING}}, {{"a", Attribute::INT64}, {"b", Attribute::INT64}});  
    bool newDB = !boost::filesystem::exists(conf.getStorageDir());
    if (!newDB) {
        try {
            boost::filesystem::remove_all(conf.getStorageDir());   
        } catch(boost::filesystem::filesystem_error const & e) {
            cout << "test FAILED\n" << endl;
            return false;
        }
    }
    boost::filesystem::create_directories(conf.getStorageDir());   
    InteractionGraph graph(conf);

    graph.createVertex(2, "v2");
    graph.createVertex(4, "v4");
    Timestamp ts = 7.0;
    bool caughtError = false;

    try {
        graph.addEdge(2, 4, ts, 1LL, 2LL);            
    } catch(runtime_error const & e) {
        cout << "test FAILED\n" << endl;
        return EXIT_FAILURE;
    }

    caughtError = false;
    try {
        graph.addEdge(2, 4, ts, 1LL, 2LL, 3LL);            
    } catch(runtime_error const & e) {
        caughtError = true;
    }
    if (!caughtError) {
        cout << "test FAILED: did not catch wrong schema size.\n" << endl;
        return EXIT_FAILURE;
    }
/*
    caughtError = false;
    try {
        graph.addEdge(2, 4, ts, "1", "2");            
    } catch(runtime_error const & e) {
        caughtError = true;
    }
    if (!caughtError) {
        cout << "test FAILED: did not catch wrong schema type.\n" << endl;
        return EXIT_FAILURE;
    }
    graph.flush();
*/
    return EXIT_SUCCESS;
}



