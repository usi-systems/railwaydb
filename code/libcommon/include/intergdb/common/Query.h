#pragma once

#include <intergdb/common/Types.h>
#include <intergdb/common/Attribute.h>

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

    bool operator<( const Query& other) const
    {        
        if (attributeNames_.size() < other.attributeNames_.size()) {
            return true;
        }

        bool lt = true;
        int i = 0;
        for (auto mine : attributeNames_) {
            lt = lt && (mine < other.attributeNames_[i]);
            i++;
        }      
        return lt;
    }
   
    std::vector<std::string> getAttributeNames() { return attributeNames_; }
    
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


} } /* namespace */


