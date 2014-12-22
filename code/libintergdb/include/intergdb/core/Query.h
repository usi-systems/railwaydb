#pragma once

#include <intergdb/common/Query.h>

namespace intergdb { namespace core
{

    class IntervalQuery : public common::Query
    {
    public:
        IntervalQuery(Timestamp start, Timestamp end, std::vector<std::string> const & attributeNames) 
            : Query(start, end, attributeNames) {} 
    };

    class FocusedIntervalQuery : public common::Query
    {
    public:
        FocusedIntervalQuery(VertexId headVertex, Timestamp start, Timestamp end, std::vector<std::string> const & attributeNames) 
            : Query(start, end, attributeNames), headVertex_(headVertex) {  }
        VertexId getHeadVertex() const { return headVertex_; }
    private:
        VertexId headVertex_;
    };
} } /* namespace */