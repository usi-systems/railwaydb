#pragma once

#include <intergdb/common/Types.h>
#include <vector>


namespace intergdb { namespace common
{
    class Query
    {
    public:
    Timestamp getStart() { return start_; }
    Timestamp getEnd() { return end_; }
    
    protected:
    Query(Timestamp start, Timestamp end, std::vector<std::string> attributeNames) 
        : start_(start), end_(end), attributeNames_(attributeNames) { } 
    std::vector<std::string> getAttributeNames() { return attributeNames_; }

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


