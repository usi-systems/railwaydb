#include <intergdb/core/QueryCollector.h>
#include <intergdb/common/QueryWorkload.h>
#include <iostream>

using namespace std;
using namespace intergdb::core;
using namespace intergdb::common;



void QueryCollector::collectIntervalQuery(Query q) 
{
    // TODO: We don't collect interval queries for now
    assert(false);
}

void QueryCollector::collectFocusedIntervalQuery(Query q) 
{
    std::vector<BucketId> bucketIds = bucketer_.getBuckets(q);
    for (auto i : bucketIds) {    
        auto iter = workloads_.find(i);
        if (iter != workloads_.end() ) {
            iter->second.addQuery(q);
        } else {
            QueryWorkload workload(conf_.getEdgeSchema().getAttributes());
            workload.addQuery(q);
            workloads_[i] = workload;       
        }       
    }   
}

