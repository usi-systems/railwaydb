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

        std::string toString() const
        {
            std::stringstream ss;
            ss << "head:" <<  headVertex_ << " ";
            ss << "start:" <<  start_ << " ";
            ss << "end:" <<  end_ << " ";
            for (auto const & a : attributeNames_) 
                ss << a << " ";
            return ss.str();
        }

    private:
        common::VertexId headVertex_;
    };
} } /* namespace */
