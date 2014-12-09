#include <intergdb/core/QueryCollector.h>
#include <intergdb/common/QueryWorkload.h>
#include <iostream>

using namespace std;
using namespace intergdb::core;
using namespace intergdb::common;

void QueryCollector::collectIntervalQuery(Query q) 
{
    std::cout << "QueryCollector::collectIntervalQuery" << std::endl;

    // We need something that takes a query, and maps it to a set of buckets
    // The bucket will be determined in different ways....
    std::vector<BucketId> bucketIds = bucketer_.getBuckets(q);
    for (auto i : bucketIds) {
        std::cout << "QueryCollector::collectIntervalQuery bucketId is " << i << std::endl;
        auto iter = workloads_.find(i);
        if (iter != workloads_.end() ) {
            std::cout << "QueryCollector::collectIntervalQuery workloads not empty" << std::endl;

            // iter->second.addQuery();
        } else {
            std::cout << "QueryCollector::collectIntervalQuery workloads empty" << std::endl;
            QueryWorkload workload; 
            conf_.getEdgeSchema();
            workload.addQuery(q);
            workloads_[i] = workload;       
        }
       
    }
    
}

void QueryCollector::collectFocusedIntervalQuery(Query q) 
{
    //std::cout << "QueryCollector::collectFocusedIntervalQuery" << std::endl;
}
