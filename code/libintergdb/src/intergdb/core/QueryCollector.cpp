#include <intergdb/core/QueryCollector.h>
#include <iostream>

using namespace std;
using namespace intergdb::core;

void QueryCollector::collectIntervalQuery(Query q) 
{
    // We need something that takes a query, and maps it to a set of buckets
    // The bucket will be determined in different ways....
    //std::cout << "QueryCollector::collectIntervalQuery" << std::endl;
    std::vector<BucketId> bucketIds = bucketer_.getBuckets(q);
    //for (auto i : bucketIds) {
    //    std::cout << "QueryCollector::collectIntervalQuery" << " " << i << std::endl;
    //}
    
}

void QueryCollector::collectFocusedIntervalQuery(Query q) 
{
    //std::cout << "QueryCollector::collectFocusedIntervalQuery" << std::endl;
}
