
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

void nov_ex1_verify(Partitioning const & partitioning)
{
    auto const & partitions = partitioning.getPartitions();
    if (partitions.size()!=2)
        throw runtime_error("nov_ex1_verify: number of partitions is not equal to 2");
    
    if (partitions[0].getAttributes().size()!=partitions[1].getAttributes().size()) 
        throw runtime_error("nov_ex1_verify: size of partitions is not equal");
    Partition const * part1 = &partitions[0];
    Partition const * part2 = &partitions[1];    
    if (part1->getAttributes().size()!=1)
         throw runtime_error("nov_ex1_verify: first partition size is not equal to 1");
    if (part2->getAttributes().size()!=1)
         throw runtime_error("nov_ex1_verify: second partition size is not equal to 1");
     
    Attribute const * smallAttribute, * largeAttribute;
    if ((*part1->getAttributes().begin())->getIndex()==(*part2->getAttributes().begin())->getIndex()) {
        throw runtime_error("nov_ex1_verify: attributes in two partitions have the same index");
    } else if ((*part1->getAttributes().begin())->getIndex()>(*part2->getAttributes().begin())->getIndex()) {
        largeAttribute = *part1->getAttributes().begin();
        smallAttribute = *part2->getAttributes().begin();
    } else {
        smallAttribute = *part1->getAttributes().begin();
        largeAttribute = *part2->getAttributes().begin();
    }    
    if (smallAttribute->getIndex()!=0)
        throw runtime_error("nov_ex1_verify: small attribute has index other than 0");
    if (largeAttribute->getIndex()!=1)
        throw runtime_error("nov_ex1_verify: large attribute has index other than 1");
}

// nov-ex2.lp 
void nov_ex2(QueryWorkload & workload, double & storageOverheadThreshold)
{
    nov_ex1(workload, storageOverheadThreshold);
    storageOverheadThreshold = 0.1;
}

void nov_ex2_verify(Partitioning const & partitioning)
{
    auto const & partitions = partitioning.getPartitions();
    if (partitions.size()!=1)
        throw runtime_error("nov_ex2_verify: number of partitions is not equal to 1");
    Partition const * part = &partitions[0];
    if (part->getAttributes().size()!=2)
         throw runtime_error("nov_ex2_verify: partition size is not equal to 2");
    auto const & attributes = part->getAttributes();
    Attribute const * smallAttribute, * largeAttribute;
    if ((*attributes.begin())->getIndex()==(*(++attributes.begin()))->getIndex()) {
        throw runtime_error("nov_ex2_verify: attributes in partition has same index");
    } else if ((*attributes.begin())->getIndex()>(*(++attributes.begin()))->getIndex()) {
        largeAttribute = *attributes.begin();
        smallAttribute = *(++attributes.begin());
    } else {
        smallAttribute = *attributes.begin();
        largeAttribute = *(++attributes.begin());
    }
    if (smallAttribute->getIndex()!=0)
        throw runtime_error("nov_ex2_verify: small attribute has index other than 0");
    if (largeAttribute->getIndex()!=1)
        throw runtime_error("nov_ex2_verify: large attribute has index other than 1");
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

void nov_ex3_verify(Partitioning const & partitioning)
{
    auto const & partitions = partitioning.getPartitions();
    if (partitions.size()!=1)
        throw runtime_error("nov_ex3_verify: number of partitions is not equal to 1");
    Partition const * part = &partitions[0];
    if (part->getAttributes().size()!=2)
         throw runtime_error("nov_ex3_verify: partition size is not equal to 2");
    auto const & attributes = part->getAttributes();
    Attribute const * smallAttribute, * largeAttribute;
    if ((*attributes.begin())->getIndex()==(*(++attributes.begin()))->getIndex()) {
        throw runtime_error("nov_ex3_verify: attributes in partition has same index");
    } else if ((*attributes.begin())->getIndex()>(*(++attributes.begin()))->getIndex()) {
        largeAttribute = *attributes.begin();
        smallAttribute = *(++attributes.begin());
    } else {
        smallAttribute = *attributes.begin();
        largeAttribute = *(++attributes.begin());
    }
    if (smallAttribute->getIndex()!=0)
        throw runtime_error("nov_ex3_verify: small attribute has index other than 0");
    if (largeAttribute->getIndex()!=1)
        throw runtime_error("nov_ex3_verify: large attribute has index other than 1");
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

void ov_ex1_verify(Partitioning const & partitioning)
{
    auto const & partitions = partitioning.getPartitions();
    if (partitions.size()!=2)
        throw runtime_error("ov_ex1_verify: number of partitions is not equal to 2");
    Partition const * smallPart, * largePart;
    if (partitions[0].getAttributes().size()==partitions[1].getAttributes().size()) {
        throw runtime_error("ov_ex1_verify: size of partitions is equal (should be different)");
    } else if (partitions[0].getAttributes().size()>partitions[1].getAttributes().size()) {
        largePart = &partitions[0];
        smallPart = &partitions[1];
    } else {
        smallPart = &partitions[0];
        largePart = &partitions[1];
    }
    if (smallPart->getAttributes().size()!=1)
         throw runtime_error("ov_ex1_verify: small partition size is not equal to 1");
    if (largePart->getAttributes().size()!=2)
         throw runtime_error("ov_ex1_verify: small partition size is not equal to 2");
     {
        auto const & attributes = largePart->getAttributes();
        Attribute const * smallAttribute, * largeAttribute;
        if ((*attributes.begin())->getIndex()==(*(++attributes.begin()))->getIndex()) {
            throw runtime_error("ov_ex1_verify: attributes in large partition has same index");
        } else if ((*attributes.begin())->getIndex()>(*(++attributes.begin()))->getIndex()) {
            largeAttribute = *attributes.begin();
            smallAttribute = *(++attributes.begin());
        } else {
            smallAttribute = *attributes.begin();
            largeAttribute = *(++attributes.begin());
        }
        if (smallAttribute->getIndex()!=0)
            throw runtime_error("ov_ex1_verify: small attribute in large partition has index other than 0");
        if (largeAttribute->getIndex()!=1)
            throw runtime_error("ov_ex1_verify: large attribute in large partition has index other than 1");
     }
     {
        auto const & attributes = smallPart->getAttributes();
        if ((*attributes.begin())->getIndex()!=1)
            throw runtime_error("ov_ex1_verify: attribute in small partition has index other than 1");
     }
}

// ov-ex2.lp 
void ov_ex2(QueryWorkload & workload, double & storageOverheadThreshold)
{
    ov_ex1(workload, storageOverheadThreshold);
    storageOverheadThreshold = 0.1;
}

void ov_ex2_verify(Partitioning const & partitioning)
{
    auto const & partitions = partitioning.getPartitions();
    if (partitions.size()!=1)
        throw runtime_error("ov_ex2_verify: number of partitions is not equal to 1");
    Partition const * part = &partitions[0];
    if (part->getAttributes().size()!=2)
         throw runtime_error("ov_ex2_verify: partition size is not equal to 2");
    auto const & attributes = part->getAttributes();
    Attribute const * smallAttribute, * largeAttribute;
    if ((*attributes.begin())->getIndex()==(*(++attributes.begin()))->getIndex()) {
        throw runtime_error("ov_ex2_verify: attributes in partition has same index");
    } else if ((*attributes.begin())->getIndex()>(*(++attributes.begin()))->getIndex()) {
        largeAttribute = *attributes.begin();
        smallAttribute = *(++attributes.begin());
    } else {
        smallAttribute = (*attributes.begin());
        largeAttribute = *(++attributes.begin());
    }
    if (smallAttribute->getIndex()!=0)
        throw runtime_error("ov_ex2_verify: small attribute has index other than 0");
    if (largeAttribute->getIndex()!=1)
        throw runtime_error("ov_ex2_verify: large attribute has index other than 1");
}

template <typename TestFunct, typename VerifyFunct, typename Solver>
bool test(TestFunct testFunc, VerifyFunct verifyFunc, Solver solver, string const & testName)
{
    QueryWorkload workload; 
    double storageOverheadThreshold;
    testFunc(workload, storageOverheadThreshold);
    cout << "Test started [" << testName << "]" << endl;
    cout << "==Query workload==\n" << workload.toString() << endl;
    cout << "==Storage overhead threshold==\n" << storageOverheadThreshold << endl;
    Partitioning partitioning = solver->solve(workload, storageOverheadThreshold); 
    cout << "==Partitioning==\n" << partitioning.toString() << "\n"<< endl;
    try {
        verifyFunc(partitioning);
    } catch(runtime_error const & e) {
        cout << e.what() << endl;
        cout << "test FAILED [" << testName << "]" << "\n" << endl;
        return false;
    }
    cout << "test SUCCESS [" << testName << "]" << "\n" << endl;
    return true;
}

int main()
{
    cout << "This is a test program for the solver." << endl;
    auto solverOv = SolverFactory::instance().makeOptimalOverlapping();    
    auto solverNov = SolverFactory::instance().makeOptimalNonOverlapping();    

    bool success = true;
    success &= test(nov_ex1, nov_ex1_verify, solverNov, "nov_ex1");
    success &= test(nov_ex2, nov_ex2_verify, solverNov, "nov_ex2");
    success &= test(nov_ex3, nov_ex3_verify, solverNov, "nov_ex3");
    success &= test(ov_ex1, ov_ex1_verify, solverOv, "ov_ex1");
    success &= test(ov_ex2, ov_ex2_verify, solverOv, "ov_ex2");

    if (success)
        return EXIT_SUCCESS;    
    else
        return EXIT_FAILURE;
}
 



