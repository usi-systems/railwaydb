
#include <cstdlib>
#include <iostream>
#include <numeric>

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>
#include <intergdb/common/Query.h>

#include <Solver.h>
#include <SolverFactory.h>

using namespace std;
using namespace intergdb::common;
using namespace intergdb::optimizer;

// nov-ex1.lp 
void nov_ex1(QueryWorkload & workload, double & storageOverheadThreshold)
{
    for (size_t i=0, iu=2; i<iu; ++i) 
        workload.addAttribute(Attribute(i, 8));    
    for (auto const & attribute : workload.getAttributes()) {
        Query query;
        query.addAttribute(attribute);
        query.setFrequency(0.5);
        workload.addQuery(query);
    }
    storageOverheadThreshold = 1.0
}

void nov_ex2(QueryWorkload & workload, double & storageOverheadThreshold)
{
    nov_ex1(workload, storageOverheadThreshold)
    storageOverheadThreshold = 0.0
}

int main()
{
    cerr << "This is a test program for the solver." << endl;
    QueryWorkload workload; 
    auto solver = SolverFactory::instance().makeOptimalOverlapping();    
    nov_ex1(workload);
    cerr << workload.toString() << endl;
    Partitioning partitioning = solver->solve(workload);
    return 0;    
}
 



