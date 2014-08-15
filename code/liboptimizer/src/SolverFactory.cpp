
#include <SolverFactory.h>
#include <OptimalOverlapping.h>
#include <OptimalNonOverlapping.h>
#include <SinglePartition.h>
#include <PartitionPerAttribute.h>
#include <HeuristicOverlapping.h>
#include <HeuristicNonOverlapping.h>

SolverFactory* SolverFactory::factory_ = NULL;

SolverFactory * SolverFactory::instance() 
{
    if (factory_ == NULL) {
        factory_ = new SolverFactory();
    }
    return factory_;
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




