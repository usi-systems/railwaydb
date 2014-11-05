#include <intergdb/core/InteractionGraph.h>
#include <intergdb/core/EdgeData.h>

#include <cstdlib>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <boost/filesystem.hpp>

using namespace std;
using namespace intergdb::core;

template <class VertexData>
unordered_map<VertexId, double> findClusteringCoefficients(
    InteractionGraph<VertexData> & graph, double startTime, double endTime)
{
    vector<VertexId> vertices;
    for (auto iqIt = graph.processIntervalQuery(startTime, endTime); 
            iqIt.isValid(); iqIt.next()) 
        vertices.push_back(iqIt.getVertexId()); 

    unordered_map<VertexId, double> clusteringCoefficients;
    for (VertexId v : vertices) {
        double clusteringCoefficient = 0.0;
        unordered_set<VertexId> tz;
        for (auto fiqIt = graph.processFocusedIntervalQuery(v, startTime, endTime); 
                fiqIt.isValid(); fiqIt.next()) {
            auto to = fiqIt.getToVertex();
            tz.insert(to);
        }
        for (VertexId const & ov : tz) {
            for (auto fiqIt = graph.processFocusedIntervalQuery(ov, startTime, endTime); 
                    fiqIt.isValid(); fiqIt.next()) {
                auto to = fiqIt.getToVertex();
                if (tz.count(to)>0)
                    clusteringCoefficient++;
            }
        }
        size_t z = tz.size();
        if (z<2)
            clusteringCoefficient = 0.0;
        else
            clusteringCoefficient /= (z*(z-1)/2.0);
        clusteringCoefficients[v] = clusteringCoefficient;
    }
    return clusteringCoefficients;
}

int main()
{
    Conf conf("test", "/tmp/myigdb_cc");
    bool newDB = !boost::filesystem::exists(conf.getStorageDir());
    boost::filesystem::create_directories(conf.getStorageDir());
    
    typedef InteractionGraph<string> Graph;
    Graph graph(conf);

    if (newDB) {  
        // TODO: create a more meaningful graph
        graph.createVertex(1, "v1"); 
        graph.createVertex(2, "v2"); 
        graph.createVertex(3, "v3");
        graph.createVertex(4, "v4");
        Timestamp ts = 1.0;
        graph.addEdge(1, 3, ts++, EdgeData("e1-3"));
        graph.addEdge(1, 4, ts++, EdgeData("e1-4"));
        graph.addEdge(2, 3, ts++, EdgeData("e2-3"));
        graph.addEdge(2, 4, ts++, EdgeData("e2-4"));
        graph.addEdge(3, 4, ts++, EdgeData("e3-4"));
        graph.flush();
    }

    double startTime = 0.0, endTime = 2.0;
    cout << "For range [" << startTime << "," << endTime << "]" << endl;
    auto clusteringCoefficients = findClusteringCoefficients(graph, startTime, endTime);
    for (auto const & idScorePair : clusteringCoefficients)
        cout << "id: " << idScorePair.first << " => cc: " << idScorePair.second << endl; 
    cout << endl;

    endTime += 5.0;
    cout << "For range [" << startTime << "," << endTime << "]" << endl;
    clusteringCoefficients = findClusteringCoefficients(graph, startTime, endTime);
    for (auto const & idScorePair : clusteringCoefficients)
        cout << "id: " << idScorePair.first << " => cc: " << idScorePair.second << endl;

    return EXIT_SUCCESS;
}



