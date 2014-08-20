
#include <intergdb/optimizer/SolverFactory.h>
#include <intergdb/optimizer/OptimalOverlapping.h>
#include <intergdb/optimizer/OptimalNonOverlapping.h>
#include <intergdb/optimizer/SinglePartition.h>
#include <intergdb/optimizer/PartitionPerAttribute.h>
#include <intergdb/optimizer/HeuristicOverlapping.h>
#include <intergdb/optimizer/HeuristicNonOverlapping.h>

using namespace std;
using namespace intergdb::optimizer;

unique_ptr<SolverFactory> SolverFactory::factory_(new SolverFactory());

SolverFactory & SolverFactory::instance() 
{
    return *factory_;
}

shared_ptr<Solver> SolverFactory::makeOptimalOverlapping()
{
    return shared_ptr<Solver>(new OptimalOverlapping());
}

shared_ptr<Solver> SolverFactory::makeOptimalNonOverlapping()
{
    return shared_ptr<Solver>(new OptimalNonOverlapping());
}

shared_ptr<Solver> SolverFactory::makeHeuristicOverlapping()
{
    return shared_ptr<Solver>(new HeuristicOverlapping());
}

shared_ptr<Solver> SolverFactory:: makeHeuristicNonOverlapping()
{
    return shared_ptr<Solver>(new HeuristicNonOverlapping());
}

shared_ptr<Solver> SolverFactory::makeSinglePartition()
{
    return shared_ptr<Solver>(new SinglePartition());
}

shared_ptr<Solver> SolverFactory::makePartitionPerAttribute()
{
    return shared_ptr<Solver>(new PartitionPerAttribute());
}




