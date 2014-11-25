#pragma once

#include <intergdb/core/Query.h>
#include <math.h>
#include <iostream>

namespace intergdb { namespace core
{

    typedef uint64_t BucketId;

    class Bucketer
    {
    protected:
    public:
        Bucketer() { }
        virtual BucketId getBucket(Query q) { 
            std::cout << "Bucketer::getBucket" << std::endl;            
                return -1; 
        }
    };

    class NaiveBucketer : public Bucketer
    {
    public:
        NaiveBucketer(int bucketSize) { }        
        virtual BucketId getBucket(Query q) { 
            std::cout << "NaiveBucketer::getBucket" << std::endl;
            Timestamp start = q.getStart();
            Timestamp end = q.getEnd();
            std::cout << "NaiveBucketer::getBucket " << start << " " << end << std::endl;
            int firstBucket = 0.0 == fmod(start, bucketSize_) ? (start / bucketSize_) :  (start / bucketSize_ + 1);
            int lastBucket = 0.0 == fmod(end, bucketSize_) ? (end / bucketSize_) :  (end / bucketSize_ + 1);
            std::cout << "NaiveBucketer::getBucket firstBucket " << firstBucket << std::endl;
            std::cout << "NaiveBucketer::getBucket lastBucket " << lastBucket << std::endl;
            return 1; 
        } 
    private:
        int bucketSize_;
    };

    class QueryCollector
    {
    public:
    QueryCollector() : bucketer_(1) { }
        void collectIntervalQuery(Query q);
        void collectFocusedIntervalQuery(Query q);
    private:
        NaiveBucketer bucketer_;
    };

} } /* namespace */


