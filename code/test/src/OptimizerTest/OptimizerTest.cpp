
#include "gtest/gtest.h"  

#include <cstdlib>
#include <iostream>
#include <numeric>

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>
#include <intergdb/common/QuerySummary.h>
#include <intergdb/common/SchemaStats.h>


#include <intergdb/optimizer/Solver.h>
#include <intergdb/optimizer/SolverFactory.h>

using namespace std;
using namespace intergdb::common;
using namespace intergdb::optimizer;



class OptimizerTest : public ::testing::Test 
{
public:
    OptimizerTest() {}
protected:
    virtual void SetUp() 
    {    
        solverNov = SolverFactory::instance().makeOptimalNonOverlapping();    
        solverOv = SolverFactory::instance().makeOptimalOverlapping();    

    }
    virtual void TearDown()
    { 
        // do nothing
    }
protected:
    std::shared_ptr<Solver> solverNov;
    std::shared_ptr<Solver> solverOv;
};

TEST_F(OptimizerTest, nov_ex1) 
{
    QueryWorkload workload;
    SchemaStats stats;
    for (size_t i=0, iu=2; i<iu; ++i) {
        workload.addAttribute(Attribute(i, std::to_string(i), intergdb::common::Attribute::UNDEFINED));
        stats.incrCountAndBytes(i, 8);
    }
    for (auto const & attribute : workload.getAttributes()) {
        QuerySummary query;
        query.addAttribute(attribute);
        workload.addQuerySummary(query);
        workload.setFrequency(query,0.5);
    }
    double storageOverheadThreshold = 1.0;
    
    Partitioning partitioning = solverNov->solve(workload, storageOverheadThreshold, stats); 

    auto const & partitions = partitioning.getPartitions();

    EXPECT_EQ(partitions.size(), 2);   
    EXPECT_EQ(partitions[0].getAttributes().size(), partitions[1].getAttributes().size());   

    
    Partition const * part1 = &partitions[0];
    Partition const * part2 = &partitions[1];    

    EXPECT_EQ(part1->getAttributes().size(),1);
    EXPECT_EQ(part2->getAttributes().size(),1);
    
    EXPECT_NE((*part1->getAttributes().begin())->getIndex(),(*part2->getAttributes().begin())->getIndex());

    Attribute const * smallAttribute, * largeAttribute;
    if ((*part1->getAttributes().begin())->getIndex()>(*part2->getAttributes().begin())->getIndex()) {
        largeAttribute = *part1->getAttributes().begin();
        smallAttribute = *part2->getAttributes().begin();
    } else {
        smallAttribute = *part1->getAttributes().begin();
        largeAttribute = *part2->getAttributes().begin();
    }    


    EXPECT_EQ (smallAttribute->getIndex(), 0);
    EXPECT_EQ (largeAttribute->getIndex(), 1);

}

TEST_F(OptimizerTest, nov_ex2) 
{
    QueryWorkload workload;
    SchemaStats stats;
    for (size_t i=0, iu=2; i<iu; ++i) {
        workload.addAttribute(Attribute(i, std::to_string(i), intergdb::common::Attribute::UNDEFINED));
        stats.incrCountAndBytes(i, 8);
    }
    for (auto const & attribute : workload.getAttributes()) {
        QuerySummary query;
        query.addAttribute(attribute);
        workload.addQuerySummary(query);
        workload.setFrequency(query,0.5);
    }
    double storageOverheadThreshold = 0.1;
    
    Partitioning partitioning = solverNov->solve(workload, storageOverheadThreshold, stats); 

    auto const & partitions = partitioning.getPartitions();
    
    EXPECT_EQ(partitions.size(), 1);   

    Partition const * part = &partitions[0];


    EXPECT_EQ(part->getAttributes().size(), 2);

    auto const & attributes = part->getAttributes();
    EXPECT_NE((*attributes.begin())->getIndex(), (*(++attributes.begin()))->getIndex());

    Attribute const * smallAttribute, * largeAttribute;
    if ((*attributes.begin())->getIndex()>(*(++attributes.begin()))->getIndex()) {
        largeAttribute = *attributes.begin();
        smallAttribute = *(++attributes.begin());
    } else {
        smallAttribute = *attributes.begin();
        largeAttribute = *(++attributes.begin());
    }

    EXPECT_EQ(smallAttribute->getIndex(), 0);
    EXPECT_EQ(largeAttribute->getIndex(), 1);
}

TEST_F(OptimizerTest, nov_ex3) 
{
    QueryWorkload workload;    
    SchemaStats stats;
    for (size_t i=0, iu=2; i<iu; ++i) {
        workload.addAttribute(Attribute(i, std::to_string(i), intergdb::common::Attribute::UNDEFINED));
        stats.incrCountAndBytes(i, 8);
    }
    for (size_t j=0, ju=2; j<ju; ++j) {
        QuerySummary query;
        for (auto const & attribute : workload.getAttributes()) 
            query.addAttribute(attribute);        
        workload.addQuerySummary(query);
        workload.setFrequency(query,0.5);
    }
    double storageOverheadThreshold = 1.0;
    Partitioning partitioning = solverNov->solve(workload, storageOverheadThreshold, stats); 
    auto const & partitions = partitioning.getPartitions();
    EXPECT_EQ(partitions.size(), 1);
    Partition const * part = &partitions[0];
    EXPECT_EQ(part->getAttributes().size(), 2);
    auto const & attributes = part->getAttributes();
    Attribute const * smallAttribute, * largeAttribute;

    EXPECT_NE((*attributes.begin())->getIndex(), (*(++attributes.begin()))->getIndex());

    if ((*attributes.begin())->getIndex()>(*(++attributes.begin()))->getIndex()) {
        largeAttribute = *attributes.begin();
        smallAttribute = *(++attributes.begin());
    } else {
        smallAttribute = *attributes.begin();
        largeAttribute = *(++attributes.begin());
    }
    EXPECT_EQ(smallAttribute->getIndex(), 0);
    EXPECT_EQ(largeAttribute->getIndex(), 1);
}

TEST_F(OptimizerTest, ov_ex1) 
{
    QueryWorkload workload;    
    SchemaStats stats;
    for (size_t i=0, iu=2; i<iu; ++i) {
        workload.addAttribute(Attribute(i, std::to_string(i), intergdb::common::Attribute::UNDEFINED));
        stats.incrCountAndBytes(i, 8);
    }
    auto const & attributes = workload.getAttributes();
    {
        QuerySummary query;
        query.addAttribute(attributes[0]);
        query.addAttribute(attributes[1]);
        workload.addQuerySummary(query);
        workload.setFrequency(query,0.5);
    }
    {
        QuerySummary query;
        query.addAttribute(attributes[1]);      
        workload.addQuerySummary(query);
        workload.setFrequency(query,0.5);
    }
    double storageOverheadThreshold = 1.0;
    Partitioning partitioning = solverOv->solve(workload, storageOverheadThreshold, stats); 
    auto const & partitions = partitioning.getPartitions();
    EXPECT_EQ(partitions.size(), 2);
    Partition const * smallPart, * largePart;
    EXPECT_NE(partitions[0].getAttributes().size(), partitions[1].getAttributes().size());

    if (partitions[0].getAttributes().size()>partitions[1].getAttributes().size()) {
        largePart = &partitions[0];
        smallPart = &partitions[1];
    } else {
        smallPart = &partitions[0];
        largePart = &partitions[1];
    }
    EXPECT_EQ(smallPart->getAttributes().size(), 1);

    EXPECT_EQ(largePart->getAttributes().size(), 2);

    {
        auto const & attributes = largePart->getAttributes();
        Attribute const * smallAttribute, * largeAttribute;
        EXPECT_NE((*attributes.begin())->getIndex(), (*(++attributes.begin()))->getIndex());
        if ((*attributes.begin())->getIndex()>(*(++attributes.begin()))->getIndex()) {
            largeAttribute = *attributes.begin();
            smallAttribute = *(++attributes.begin());
        } else {
            smallAttribute = *attributes.begin();
            largeAttribute = *(++attributes.begin());
        }
        EXPECT_EQ(smallAttribute->getIndex(), 0);
        EXPECT_EQ(largeAttribute->getIndex(), 1);
     }
     {
        auto const & attributes = smallPart->getAttributes();
        EXPECT_EQ((*attributes.begin())->getIndex(), 1);
     }
}

TEST_F(OptimizerTest, ov_ex2) 
{

    QueryWorkload workload;    
    SchemaStats stats;
    for (size_t i=0, iu=2; i<iu; ++i) {
        workload.addAttribute(Attribute(i, std::to_string(i), intergdb::common::Attribute::UNDEFINED));
        stats.incrCountAndBytes(i, 8);
    }
    auto const & attributes = workload.getAttributes();
    {
        QuerySummary query;
        query.addAttribute(attributes[0]);
        query.addAttribute(attributes[1]);
        workload.addQuerySummary(query);
        workload.setFrequency(query,0.5);
    }
    {
        QuerySummary query;
        query.addAttribute(attributes[1]);      
        workload.addQuerySummary(query);
        workload.setFrequency(query,0.5);
    }
    double storageOverheadThreshold = 0.1;
    Partitioning partitioning = solverOv->solve(workload, storageOverheadThreshold, stats); 
        auto const & partitions = partitioning.getPartitions();
    if (partitions.size()!=1)
        throw runtime_error("ov_ex2_verify: number of partitions is not equal to 1");
    Partition const * part = &partitions[0];
    EXPECT_EQ(part->getAttributes().size(), 2);
    {
        auto const & attributes = part->getAttributes();
        Attribute const * smallAttribute, * largeAttribute;
        EXPECT_NE((*attributes.begin())->getIndex(), (*(++attributes.begin()))->getIndex());
        if ((*attributes.begin())->getIndex()>(*(++attributes.begin()))->getIndex()) {
            largeAttribute = *attributes.begin();
            smallAttribute = *(++attributes.begin());
        } else {
            smallAttribute = (*attributes.begin());
            largeAttribute = *(++attributes.begin());
        }
        EXPECT_EQ(smallAttribute->getIndex(), 0);    
        EXPECT_EQ(largeAttribute->getIndex(), 1);
    }
}

