#include <intergdb/core/InteractionGraph.h>

#include <cstdlib>
#include <iostream>

using namespace std;
using namespace intergdb::core;

int main()
{
    Conf conf("test", "/tmp/");
    typedef InteractionGraph<std::string,std::string> Graph;
    Graph graph(conf);
    graph.createVertex(2, "");
    graph.createVertex(4, "");
    graph.addEdge(2, 4, "");
    graph.flush();
    graph.getVertexData(2);
    Graph::VertexIterator iqIt = graph.processIntervalQuery(5, 10);
    while(iqIt.isValid()) {
        iqIt.getVertexData();
        iqIt.getVertexId();
        iqIt.next();
    }
    Graph::EdgeIterator fiqIt = graph.processFocusedIntervalQuery(0, 5, 10);
    while(fiqIt.isValid()) {
        fiqIt.getEdgeData();
        fiqIt.getToVertex();
        fiqIt.getTime();
        fiqIt.next();
    }
    return EXIT_SUCCESS;
}



