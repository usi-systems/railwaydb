#pragma once

#include <intergdb/core/Query.h>

#include <iostream>

namespace intergdb { namespace core
{

    typedef uint64_t BucketId;

    class Bucketer
    {
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
        NaiveBucketer() {}
        virtual BucketId getBucket(Query q) { 
            std::cout << "NaiveBucketer::getBucket" << std::endl;
            return 1; 
        } 
    };

    class QueryCollector
    {
    public:
    QueryCollector() : bucketer_(NaiveBucketer()) {}
        void collectIntervalQuery(Query q);
        void collectFocusedIntervalQuery(Query q);
    private:
        // TODO: (rjs): change this to a pointer to a bucketer?
        Bucketer bucketer_;
    };

} } /* namespace */


