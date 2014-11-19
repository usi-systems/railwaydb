#include <intergdb/core/InteractionGraph.h>
#include <intergdb/core/Schema.h>

#include <cstdlib>
#include <iostream>
#include <climits>
#include <set>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace intergdb::core;
using boost::lexical_cast;

#define MAX_WT std::numeric_limits<double>::max()

void createGraph(InteractionGraph &graph) 
{
    graph.createVertex(0, "v0");
    graph.createVertex(1, "v1");
    graph.createVertex(2, "v2");
    graph.createVertex(3, "v3");
    graph.createVertex(4, "v4");
    graph.createVertex(5, "v5");
    graph.createVertex(6, "v6");
    graph.createVertex(7, "v7");
    Timestamp ts = 1.0;
    graph.addEdge(3, 5, ts, "1"); 
    graph.addEdge(0, 2, ts, "2");
    graph.addEdge(0, 7, ts, "3");
    graph.addEdge(0, 1, ts, "4");
    graph.addEdge(3, 4, ts, "5");
    graph.addEdge(5, 4, ts, "6"); 
    graph.addEdge(0, 6, ts, "7");
    graph.addEdge(7, 1, ts, "8");
    graph.addEdge(7, 6, ts, "9");
    graph.addEdge(7, 4, ts, "10");
    graph.addEdge(6, 4, ts, "11");
    graph.addEdge(0, 5, ts, "12");
    graph.flush();     
}

int msp(InteractionGraph &graph, VertexId q0, set<pair<VertexId,VertexId> > &tree,
        double startTime, double endTime, std::vector<std::string> attributes)
{
    std::set<VertexId> out; // The set of vertices not in the tree
    std::set<VertexId> in;  // The set of vertices in the tree
    VertexId v;
    VertexId u;

    int i = 0;
    IntervalQuery q1(startTime, endTime, attributes);
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
                    edgeWeight = lexical_cast<double>(*fiqIt.getEdgeData());
                    if (edgeWeight <= minWeight) {
                        minWeight = edgeWeight;
                        minDst = v;
                        minSrc = u;
                    }                                       
                }
            }
        }
        tree.insert(std::make_pair (minSrc,minDst));        
    }

    return 0;
}

int main()
{
    Conf conf("test", "/tmp/myigdb_st", {{"vertex-label",Schema::STRING}}, {{"a", Schema::STRING}});
    bool newDB = !boost::filesystem::exists(conf.getStorageDir());
    boost::filesystem::create_directories(conf.getStorageDir());

    set<pair<VertexId,VertexId> > tree;
    InteractionGraph graph(conf);

    if (newDB) {  
        createGraph(graph);
    }

    double startTime = 0.0, endTime = 2.0;
    std::vector<std::string> attributes = {"a"};
    msp(graph, 0, tree, startTime, endTime, attributes);

    for (auto it=tree.begin(); it!=tree.end(); ++it) {
        cout << it->first << "->" <<it->second << " in tree" << endl;
    }

    return EXIT_SUCCESS;
}



