#pragma once

#include <intergdb/common/Query.h>
#include <intergdb/common/QueryWorkload.h>
#include <intergdb/core/Conf.h>

#include <math.h>
#include <map>
#include <iostream>

#include <limits>

using namespace intergdb::common;

namespace intergdb { namespace core
{
    typedef uint64_t BucketId;

    class Bucketer
    {
    public:
        Bucketer()
        {}
        virtual std::vector<BucketId> getBuckets(Query const & q) = 0;
    };

    class NaiveBucketer : public Bucketer
    {
    public:
        NaiveBucketer(int bucketSize)
            : bucketSize_(bucketSize)
        {}

        std::vector<BucketId> getBuckets(Query const & q) override
        {
            std::vector<BucketId> bucketIds;
            Timestamp start = q.getStartTime();
            Timestamp end = q.getEndTime();
            int firstBucket = 0.0 == fmod(start, bucketSize_)
                ? (start / bucketSize_) :  (start / bucketSize_ + 1);
            int lastBucket = 0.0 == fmod(end, bucketSize_)
                ? (end / bucketSize_) :  (end / bucketSize_ + 1);
            for (int i = firstBucket; i <= lastBucket; i += bucketSize_)
                bucketIds.push_back(i);
            return bucketIds;
        }
    private:
        int bucketSize_;
    };

    class OneBucketer : public Bucketer
    {
    public:
        OneBucketer()
        {}

        std::vector<BucketId> getBuckets(Query const & q) override
        {
            std::vector<BucketId> bucketIds;
            bucketIds.push_back(0);
            return bucketIds;
        }
    };

    class QueryCollector
    {
    public:
        QueryCollector(Conf const & conf)
            : conf_(conf), bucketer_()
        {}

        void collectIntervalQuery(Query const & q);

        void collectFocusedIntervalQuery(Query const & q);

        std::map<BucketId,common::QueryWorkload> & getWorkloads()
        {
            return workloads_;
        }

        void reset()
        {
            workloads_.clear();
        }

    private:
        Conf const & conf_;
        OneBucketer bucketer_;
        std::map<BucketId,common::QueryWorkload> workloads_;
    };
} } /* namespace */
