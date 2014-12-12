#include <intergdb/core/InteractionGraph.h>
#include <intergdb/common/Attribute.h>

#include <cstdlib>
#include <iostream>


#include <boost/filesystem.hpp>

using namespace std;
using namespace intergdb::core;
using namespace intergdb::common;

int main()
{
    Conf conf("test", "/tmp/myigdb", {{"vertex-label",Attribute::STRING}}, {{"a", Attribute::STRING}, {"b", Attribute::STRING}});
    bool newDB = !boost::filesystem::exists(conf.getStorageDir());
    boost::filesystem::create_directories(conf.getStorageDir());   
    InteractionGraph graph(conf);

    if (newDB) {  
        graph.createVertex(2, "v2");
        graph.createVertex(4, "v4");
        Timestamp ts = 7.0;
        graph.addEdge(2, 4, ts, "e2-4", "b-data");
        graph.flush();
    }

    IntervalQuery q1(5.0, 10.0, {"a"});
    IntervalQuery q2(5.0, 10.0, {"b"});
    IntervalQuery q3(5.0, 10.0, {"a"});
    FocusedIntervalQuery q4(2, 5.0, 10.0, {"a"});

    InteractionGraph::VertexIterator iqIt = graph.processIntervalQuery(q1);
    while(iqIt.isValid()) {
        cout << *iqIt.getVertexData() << endl; 
        cout << iqIt.getVertexId() << endl; 
        iqIt.next();
    }

    graph.processIntervalQuery(q2);
    graph.processIntervalQuery(q3);
/*
    InteractionGraph::EdgeIterator fiqIt = graph.processFocusedIntervalQuery(q4);
    while(fiqIt.isValid()) {
        cout << *fiqIt.getEdgeData() << endl;  
        cout << fiqIt.getToVertex() << endl; 
        cout << fiqIt.getTime() << endl; 
        fiqIt.next();
    }
*/
    return EXIT_SUCCESS;
}



