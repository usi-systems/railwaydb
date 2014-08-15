#pragma once

#include <Solver.h>

#include <memory>

namespace intergdb { namespace optimizer {

class SolverFactory
{
public:
    static SolverFactory & instance(); // TODO: this better returns a reference
    Solver * makeOptimalOverlapping(); // TODO: These could all return references
    Solver * makeOptimalNonOverlapping();
    Solver * makeHeuristicOverlapping();
    Solver * makeHeuristicNonOverlapping();
    Solver * makeSinglePartition();
    Solver * makePartitionPerAttribute();

private:
    SolverFactory() {}
    static std::unique_ptr<SolverFactory> factory_;
};

} }