
#include <SolverFactory.h>
#include <OptimalOverlapping.h>
#include <OptimalNonOverlapping.h>


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
    return NULL;
}

Solver * SolverFactory:: makeHeuristicNonOverlapping()
{
    return NULL;
}

Solver * SolverFactory::makeSinglePartition()
{
    return NULL;
}

Solver * SolverFactory::makePartitionPerAttribute()
{
    return NULL;
}




