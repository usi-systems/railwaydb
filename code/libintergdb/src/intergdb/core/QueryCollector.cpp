#include <intergdb/core/QueryCollector.h>
#include <intergdb/common/QueryWorkload.h>
#include <iostream>

using namespace std;
using namespace intergdb::core;
using namespace intergdb::common;

#undef DEBUG 

void QueryCollector::collectIntervalQuery(Query q) 
{
    #ifdef DEBUG
    std::cout << "QueryCollector::collectIntervalQuery" << std::endl;
    #endif
    // We need something that takes a query, and maps it to a set of buckets
    // The bucket will be determined in different ways....
    std::vector<BucketId> bucketIds = bucketer_.getBuckets(q);
    for (auto i : bucketIds) {
    #ifdef DEBUG
        std::cout << "QueryCollector::collectIntervalQuery bucketId is " << i << std::endl;
    #endif
        auto iter = workloads_.find(i);
        if (iter != workloads_.end() ) {
   #ifdef DEBUG
            std::cout << "QueryCollector::collectIntervalQuery workloads not empty" << std::endl;
    #endif
            // iter->second.addQuery();
        } else {
    #ifdef DEBUG
            std::cout << "QueryCollector::collectIntervalQuery workloads empty" << std::endl;         
    #endif
            // Schema schema = conf_.getEdgeSchema();
            QueryWorkload workload(conf_.getEdgeSchema().getAttributes());
            workload.addQuery(q);
            workloads_[i] = workload;       
        }
       
    }
    
}

void QueryCollector::collectFocusedIntervalQuery(Query q) 
{
    //std::cout << "QueryCollector::collectFocusedIntervalQuery" << std::endl;
}
