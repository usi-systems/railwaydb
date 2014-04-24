#include <intergdb/core/InteractionGraph.h>

#include <cstdlib>
#include <iostream>
#include <climits>
#include <vector>
#include <map>

#include <boost/lexical_cast.hpp>

#include <boost/filesystem.hpp>

using namespace std;
using namespace intergdb::core;
using boost::lexical_cast;

typedef InteractionGraph<std::string,std::string> Graph;


class Key
{
  public: 
    VertexId s;
    VertexId d;
    Key(VertexId s, VertexId d)
    {
            this->s = s;
            this->d = d;
    }
    bool operator<(const Key& k) const
    {
      int s_cmp = this->s < k.s;
      if(s_cmp == 0)
      {
        return this->d < k.d;
      }
      return s_cmp < 0;
    }
};


void createGraph(Graph &graph) 
{
    // The example from page 236 of 
    // Algorithms in C by Sedgewick
    graph.createVertex(0, "v0");
    graph.createVertex(1, "v1");
    graph.createVertex(2, "v2");
    graph.createVertex(3, "v3");
    graph.createVertex(4, "v4");
    graph.createVertex(5, "v5");
    graph.createVertex(6, "v6");
    graph.createVertex(7, "v7");
    Timestamp ts = 1.0;
    graph.addEdge(3, 5, "1", ts); 
    graph.addEdge(0, 2, "2", ts);
    graph.addEdge(0, 7, "3", ts);
    graph.addEdge(0, 1, "4", ts);
    graph.addEdge(3, 4, "5", ts);
    graph.addEdge(5, 4, "6", ts); 
    graph.addEdge(0, 6, "7", ts);
    graph.addEdge(7, 1, "8", ts);
    graph.addEdge(7, 6, "9", ts);
    graph.addEdge(7, 4, "10", ts);
    graph.addEdge(6, 4, "11", ts);
    graph.addEdge(0, 5, "12", ts);

    graph.flush();     
}


void print_wt( map<VertexId,double> &wt) {
    VertexId key;
    double value;
    cout << "wt: " << endl;
    for ( auto iter : wt ) {
        key=iter.first;
        value=iter.second;
        cout << "  wt[" << key << "]=" << value << endl;
    }
    cout << endl;
}

void print_st( map<VertexId,VertexId> &st) {
    VertexId key, value;
    cout << "st: " << endl;
    for ( auto iter : st ) {
        key=iter.first;
        value=iter.second;
        cout << "  st[" << key << "]=" << value << endl;
    }
}

void print_adj( map<Key,double> adj) {
    //Key key;
    double value;
    cout << "adj: " << endl;
    for ( auto iter : adj ) {
        //key=iter.first;
        value=iter.second;
        cout << "  adj[" << iter.first.s << "][" << iter.first.d << "]=" << value << endl;
    }
}

#define NaN std::numeric_limits<uint64_t>::max()
#define MAX_WT std::numeric_limits<double>::max()


int msp(Graph &graph, map<VertexId,VertexId> &st, map<VertexId,double> &wt, double startTime, double endTime)
{

    vector<VertexId> vertices;
    map<Key,double> adj;

    for (auto iqIt = graph.processIntervalQuery(startTime, endTime); 
         iqIt.isValid(); iqIt.next()) 
        vertices.push_back(iqIt.getVertexId()); 
    std::cout << "number of vertices is " << vertices.size() << std::endl;
    std::cout << "number of st is " << st.size() << std::endl;
    std::cout << "number of wt is " << wt.size() << std::endl;

    for (VertexId v : vertices) {
        for (VertexId w : vertices) {
            adj[*(new Key(v,w))] = MAX_WT;
        }
    }

    for (VertexId v : vertices) {
        Graph::EdgeIterator fiqIt = graph.processFocusedIntervalQuery(v, startTime, endTime);
        while(fiqIt.isValid()) {
            adj[*(new Key(v,fiqIt.getToVertex()))] = lexical_cast<double>(*fiqIt.getEdgeData());
            adj[*(new Key(fiqIt.getToVertex(),v))] = lexical_cast<double>(*fiqIt.getEdgeData());

            cout << "  adj[" << v << "][" << fiqIt.getToVertex() << "]=" << lexical_cast<double>(*fiqIt.getEdgeData()) << endl;


            fiqIt.next();
        }        
    }

    vector<VertexId> fr(vertices.size());

    VertexId v, w, min;
    for (VertexId v : vertices) {
        std::cout << ">>> st[" <<  v << "]<<<" << std::endl;
        st[v] = NaN; // need -1 or some sentinal
        fr[v] = v;
        wt[v] = MAX_WT;
        std::cout << ">>> st[" <<  v << "]=" << st[v] <<  std::endl;

    }

    print_st(st);
    print_wt(wt);

    st[0] = 0;
    wt[vertices.size()] = INT_MAX;

    Key k (0,0);


    print_adj(adj);


    for (min = 0; min != vertices.size();) {
        v = min;
        st[min] = fr[min];

        std::cout << "st[" << min << "]=" << fr[min] << endl;
        std::cout << "v=" << v << ", min=" << min << endl;

        for (w = 0, min = vertices.size(); w < vertices.size(); w++) {
            std::cout << "----------------------------------" << endl;

            std::cout << "w=" << w << ", min=" << min << endl;

            if (st[w] == NaN) 
            {
                k.s = v;
                k.d = w;
                std::cout << "adj[" <<  v << "," << w << "]=" << adj[k] << std::endl;
                std::cout << "wt[" <<  w << "]=" << wt[w] << std::endl;


                if (adj[k] < wt[w])
                { 
                    wt[w] = adj[k];
                    fr[w] = v;
                }
                if ( wt[w] < wt[min]) 
                {
                    min = w;
                }
            }
        }
    }
    
    print_st(st);
    print_wt(wt);
    print_adj(adj);
    
    return 0;
}

int main()
{
    Conf conf("test", "/tmp/myigdb_msp");
    bool newDB = !boost::filesystem::exists(conf.getStorageDir());
    boost::filesystem::create_directories(conf.getStorageDir());
    map<VertexId,VertexId> st;
    map<VertexId,double> wt;

    Graph graph(conf);

    if (newDB) {  
        createGraph(graph);
    }

    double startTime = 0.0, endTime = 2.0;
    msp(graph, st, wt, startTime, endTime);

    return EXIT_SUCCESS;
}



