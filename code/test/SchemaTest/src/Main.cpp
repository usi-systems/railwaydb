#include <intergdb/core/InteractionGraph.h>
#include <intergdb/core/Schema.h>

#include <cstdlib>
#include <iostream>


#include <boost/filesystem.hpp>

using namespace std;
using namespace intergdb::core;

int main()
{
    Conf conf("test", "/tmp/st_igdb", {{"vertex-label",Schema::STRING}}, {{"a", Schema::INT64}, {"b", Schema::INT64}});  
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
    
    try {
        graph.addEdge(2, 4, ts, "1");            
    } catch(runtime_error const & e) {
        // TODO: This should be an error
    }
    try {
        graph.addEdge(2, 4, ts, "1", "2", "3");            
    } catch(runtime_error const & e) {
        // TODO: This should be an error
    }

    graph.flush();
    
    return EXIT_SUCCESS;
}



