#pragma once

#include <Solver.h>

class SolverFactory
{
public:
    static SolverFactory * instance();
    Solver * makeOptimalOverlapping();
    Solver * makeOptimalNonOverlapping();
    Solver * makeHeuristicOverlapping();
    Solver * makeHeuristicNonOverlapping();
    Solver * makeSinglePartition();
    Solver * makePartitionPerAttribute();

private:
    SolverFactory() {}
    static SolverFactory* factory_;
};

