#include <intergdb/core/InteractionGraph.h>

#include <cstdlib>
#include <iostream>


#include <boost/filesystem.hpp>

using namespace std;
using namespace intergdb::core;

int main()
{
    Conf conf("test", "/tmp/myigdb");
    bool newDB = !boost::filesystem::exists(conf.getStorageDir());
    boost::filesystem::create_directories(conf.getStorageDir());   
    InteractionGraph graph(conf);
    graph.getVertexSchema().addAttribute("vertex-label", Schema::STRING);
    graph.getEdgeSchema().addAttribute("edge-label", Schema::STRING);

    if (newDB) {  
        graph.createVertex(2, "v2");
        graph.createVertex(4, "v4");
        Timestamp ts = 7.0;
        graph.addEdge(2, 4, ts, "e2-4");
        graph.flush();
    }

    InteractionGraph::VertexIterator iqIt = graph.processIntervalQuery(5.0, 10.0);
    while(iqIt.isValid()) {
        cout << *iqIt.getVertexData() << endl; 
        cout << iqIt.getVertexId() << endl; 
        iqIt.next();
    }
    InteractionGraph::EdgeIterator fiqIt = graph.processFocusedIntervalQuery(2, 5.0, 10.0);
    while(fiqIt.isValid()) {
        cout << *fiqIt.getEdgeData() << endl;  
        cout << fiqIt.getToVertex() << endl; 
        cout << fiqIt.getTime() << endl; 
        fiqIt.next();
    }
    return EXIT_SUCCESS;
}



