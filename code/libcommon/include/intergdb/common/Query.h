#pragma once

#include <intergdb/common/Types.h>
#include <intergdb/common/Attribute.h>
#include <boost/functional/hash.hpp>

#include <vector>
#include <algorithm>
#include <sstream> 

namespace intergdb { namespace common
{
    class Query
    {
    public:
    Timestamp getStart() { return start_; }
    Timestamp getEnd() { return end_; }

    bool operator==( const Query& other) const
    {        
        if (attributeNames_.size() != other.attributeNames_.size()) {
            return false;
        }
        int i = 0;
        for (auto mine : attributeNames_) {
            if (mine != other.attributeNames_[i]) 
                return false;
            i++;
        }      
        return true;
    }
   
    std::vector<std::string> getAttributeNames() const { return attributeNames_; }
    
    std::string toString() const
    {
        std::stringstream ss;
        for (auto a : attributeNames_) {
            ss << a << " ";
        }
        return ss.str();
    }

    protected:
    Query(Timestamp start, Timestamp end, std::vector<std::string> attributeNames) 
        : start_(start), end_(end), attributeNames_(attributeNames) 
    { 
        std::sort(attributeNames.begin(), attributeNames.end());
    } 

    

    protected:
    Timestamp start_;
    Timestamp end_;
    std::vector<std::string> attributeNames_;   
    };


    class IntervalQuery : public Query
    {
    public:
    IntervalQuery(Timestamp start, Timestamp end, std::vector<std::string> attributeNames) 
        :  Query(start, end, attributeNames) {  } 
    };

    class FocusedIntervalQuery : public Query
    {
    public:
    FocusedIntervalQuery(VertexId headVertex, Timestamp start, Timestamp end, std::vector<std::string> attributeNames) 
        : Query(start, end, attributeNames), headVertex_(headVertex) {  }
        VertexId getHeadVertex() { return headVertex_; }
    private:
        VertexId headVertex_;
    };

    struct QueryHasher 
    {
        std::size_t operator()(const Query& q) const
        {
            using boost::hash_value;
            using boost::hash_combine;
            
            // Start with a hash value of 0    .
            std::size_t seed = 0;
            
            // Modify 'seed' by XORing and bit-shifting in
            // one member of 'Key' after the other:
            for (auto attr : q.getAttributeNames()) {
                hash_combine(seed,hash_value(attr));
            }
            
            // Return the result.
            return seed;
        }        
    };


} } /* namespace */


