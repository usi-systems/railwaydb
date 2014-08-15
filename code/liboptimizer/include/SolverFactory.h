#pragma once

#include <Solver.h>

#include <memory>

namespace intergdb { namespace optimizer {

class SolverFactory
{
public:
    static SolverFactory & instance(); // TODO: this better returns a reference
    std::shared_ptr<Solver> makeOptimalOverlapping(); // TODO: These could all return references
    std::shared_ptr<Solver> makeOptimalNonOverlapping();
    std::shared_ptr<Solver> makeHeuristicOverlapping();
    std::shared_ptr<Solver> makeHeuristicNonOverlapping();
    std::shared_ptr<Solver> makeSinglePartition();
    std::shared_ptr<Solver> makePartitionPerAttribute();

private:
    SolverFactory() {}
    static std::unique_ptr<SolverFactory> factory_;
};

} }