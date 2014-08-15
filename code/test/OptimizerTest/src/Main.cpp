
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
    storageOverheadThreshold = 1.0;
}

// nov-ex2.lp 
void nov_ex2(QueryWorkload & workload, double & storageOverheadThreshold)
{
    nov_ex1(workload, storageOverheadThreshold);
    storageOverheadThreshold = 0.0;
}

// nov-ex3.lp 
void nov_ex3(QueryWorkload & workload, double & storageOverheadThreshold)
{
    for (size_t i=0, iu=2; i<iu; ++i) 
        workload.addAttribute(Attribute(i, 8));    
    for (size_t j=0, ju=2; j<ju; ++j) {
        Query query;
        for (auto const & attribute : workload.getAttributes()) 
            query.addAttribute(attribute);        
        query.setFrequency(0.5);
        workload.addQuery(query);
    }
    storageOverheadThreshold = 1.0;
}

// ov-ex1.lp 
void ov_ex1(QueryWorkload & workload, double & storageOverheadThreshold)
{
    for (size_t i=0, iu=2; i<iu; ++i) 
        workload.addAttribute(Attribute(i, 8));    
    auto const & attributes = workload.getAttributes();
    {
        Query query;
        query.addAttribute(attributes[0]);
        query.addAttribute(attributes[1]);
        query.setFrequency(0.5);
        workload.addQuery(query);
    }
    {
        Query query;
        query.addAttribute(attributes[1]);
        query.setFrequency(0.5);
        workload.addQuery(query);
    }
    storageOverheadThreshold = 1.0;
}

// ov-ex2.lp 
void ov_ex2(QueryWorkload & workload, double & storageOverheadThreshold)
{
    ov_ex1(workload, storageOverheadThreshold);
    storageOverheadThreshold = 0.0;
}

template <typename TestFunct, typename Solver>
void test(TestFunct testFunc, Solver solver)
{
    QueryWorkload workload; 
    double storageOverheadThreshold;
    testFunc(workload, storageOverheadThreshold);
    cout << "query workload: " << workload.toString() << endl;
    cout << "storage overhead threshold: " << storageOverheadThreshold << endl;
    Partitioning partitioning = solver->solve(workload); //, storageOverheadThreshold);
    // TODO: verification
}

int main()
{
    cerr << "This is a test program for the solver." << endl;
    auto solverOv = SolverFactory::instance().makeOptimalOverlapping();    
    auto solverNov = SolverFactory::instance().makeOptimalNonOverlapping();    

    test(nov_ex1, solverNov);
    test(nov_ex2, solverNov);
    test(nov_ex3, solverNov);
    test(ov_ex1, solverOv);
    test(ov_ex2, solverOv);

    return EXIT_SUCCESS;    
}
 



