#pragma once

#include <intergdb/common/Query.h>
#include <intergdb/common/Types.h>


namespace intergdb { namespace core
{

    class IntervalQuery : public common::Query
    {
    public:
    IntervalQuery(common::Timestamp start, common::Timestamp end) 
            : Query(start, end, std::vector<std::string>()) {} 
    };

    class FocusedIntervalQuery : public common::Query
    {
    public:
    FocusedIntervalQuery(common::VertexId headVertex, common::Timestamp start, common::Timestamp end, std::vector<std::string> const & attributeNames) 
            : Query(start, end, attributeNames), headVertex_(headVertex) {  }
        common::VertexId getHeadVertex() const { return headVertex_; }
    private:
        common::VertexId headVertex_;
    };
} } /* namespace */
