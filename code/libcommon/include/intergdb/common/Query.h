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

    size_t getHashCode() const {
        return hash_;
    }
  
    protected:
    Query(Timestamp start, Timestamp end, std::vector<std::string> attributeNames) 
        : start_(start), end_(end), attributeNames_(attributeNames) 
    { 
        std::sort(attributeNames.begin(), attributeNames.end());
        hash_ = hash();
    } 
   
    protected:
    Timestamp start_;
    Timestamp end_;
    std::vector<std::string> attributeNames_;   
    size_t hash_;
    
    private:
    std::size_t hash() {
        using boost::hash_value;
        using boost::hash_combine;        
        std::size_t seed = 0;
        for (auto attr : attributeNames_) {
            hash_combine(seed,hash_value(attr));
        }
        return seed;
    }
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

} } /* namespace */


namespace std { 
    template<>
        struct hash<intergdb::common::Query> {
        inline std::size_t operator()(const intergdb::common::Query& q) const
        {
            return q.getHashCode();
        }        

    };
}
