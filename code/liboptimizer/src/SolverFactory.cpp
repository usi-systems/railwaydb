
#include <SolverFactory.h>
#include <OptimalOverlapping.h>
#include <OptimalNonOverlapping.h>
#include <SinglePartition.h>
#include <PartitionPerAttribute.h>
#include <HeuristicOverlapping.h>
#include <HeuristicNonOverlapping.h>

using namespace std;
using namespace intergdb::optimizer;

unique_ptr<SolverFactory> SolverFactory::factory_(new SolverFactory());

SolverFactory & SolverFactory::instance() 
{
    return *factory_;
}

Solver * SolverFactory::makeOptimalOverlapping()
{
    return new OptimalOverlapping();
}

Solver * SolverFactory::makeOptimalNonOverlapping()
{
    return new OptimalNonOverlapping();
}

Solver * SolverFactory::makeHeuristicOverlapping()
{
    return new HeuristicOverlapping();
}

Solver * SolverFactory:: makeHeuristicNonOverlapping()
{
    return new HeuristicNonOverlapping();
}

Solver * SolverFactory::makeSinglePartition()
{
    return new SinglePartition();
}

Solver * SolverFactory::makePartitionPerAttribute()
{
    return new PartitionPerAttribute();
}




