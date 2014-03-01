#include <intergdb/run/test/RunTest.h>

#include <intergdb/gen/InteractionGraphGenerator.h>

#include <iostream>
#include <stdexcept>

using namespace std;
using namespace intergdb::gen;
using namespace intergdb::run;
using namespace intergdb::run::test;
using namespace intergdb::core;

void RunGeneratorTest::process()
{
    InteractionGraphGenerator gen;
    auto & conf = gen.conf();

    conf.graphType() = InteractionGraphGenerator::ErdosReyni;
    conf.numVertices() = 100*1000;
    conf.numEdges() = 10*conf.numVertices();
    conf.interArrivalTimeMean() = InteractionGraphGenerator::Conf::
            convertPerDayCountToInterArrivalDelayInMillisecs(10*conf.numVertices());

    cout << "Building generative graph..." << endl;
    /*
    gen.buildGraph();
    cout << "Built generative graph." << endl;

    for (size_t i=0; i<1000*1000; ++i) {
        auto edge = gen.generateNextEdge();
        cout << "(" << edge.getFromVertex()
             << "," << edge.getToVertex()
             << "," << edge.getTime() << ")" << endl;
    }
     */
    conf.graphType() = InteractionGraphGenerator::ScaleFree;

    cout << "Building generative graph..." << endl;
    gen.buildGraph();
    cout << "Built generative graph." << endl;

    for (size_t i=0; i<10; ++i) { // 1000*1000; ++i) {
        auto edge = gen.generateNextEdge();
        cout << "(" << edge.getFromVertex()
             << "," << edge.getToVertex()
             << "," << edge.getTime() << ")" << endl;
    }

    cout << "Building generative graph..." << endl;
    gen.buildGraph();
    cout << "Built generative graph." << endl;

    for (size_t i=0; i<10; ++i) { // 1000*1000; ++i) {
        auto edge = gen.generateNextEdge();
        cout << "(" << edge.getFromVertex()
             << "," << edge.getToVertex()
             << "," << edge.getTime() << ")" << endl;
    }

    return;


    conf.graphType() = InteractionGraphGenerator::ErdosReyni;
    conf.numVertices() = 100;
    conf.numEdges() = 10*conf.numVertices();
    conf.interArrivalTimeMean() = InteractionGraphGenerator::Conf::
            convertPerDayCountToInterArrivalDelayInMillisecs(10*conf.numVertices());

    cout << "Building generative graph..." << endl;
    gen.buildGraph();
    cout << "Built generative graph." << endl;

    for (size_t i=0; i<1000*1000; ++i) {
        auto edge = gen.generateNextEdge();
        cout << "(" << edge.getFromVertex()
             << "," << edge.getToVertex()
             << "," << edge.getTime() << ")" << endl;
    }

    auto it = gen.getVertexIterator();
    for (; it.isValid(); it.next()) {
        VertexId id = it.getVertex();
        cout << id << endl;
    }
}

