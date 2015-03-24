#pragma once

#include <intergdb/common/Types.h>
#include <intergdb/common/Attribute.h>

#include <algorithm>
#include <sstream>
#include <vector>


namespace intergdb { namespace common
{
    class Query
    {
    public:
        Timestamp getStartTime() const
        {
            return start_;
        }

        Timestamp getEndTime() const
        {
            return end_;
        }

        bool operator==(Query const & other) const
        {
            if (attributeNames_.size() != other.attributeNames_.size())
                return false;
            size_t i = 0;
            for (auto const & mine : attributeNames_) {
                if (mine != other.attributeNames_[i])
                    return false;
                i++;
            }
            return true;
        }

        std::vector<std::string> const & getAttributeNames() const
        {
            return attributeNames_;
        }

        std::string toString() const
        {
            std::stringstream ss;
            for (auto const & a : attributeNames_)
                ss << a << " ";
            return ss.str();
        }

        size_t getHashCode() const
        {
            return hash_;
        }

    protected:
        Query(Timestamp start, Timestamp end, std::vector<std::string> const & attributeNames)
            : start_(start), end_(end), attributeNames_(attributeNames)
        {
            std::sort(attributeNames_.begin(), attributeNames_.end());
            hash_ = hash();
        }

    protected:
        Timestamp start_;
        Timestamp end_;
        std::vector<std::string> attributeNames_;
        size_t hash_;

    private:
        std::size_t hash()
        {
            size_t hval = 17;
            for (auto attr : attributeNames_)
                hval = hval * 37 + std::hash<std::string>()(attr);
            return hval;
        }
    };

} } /* namespace */


namespace std {
    template<>
    struct hash<intergdb::common::Query>
    {
        inline std::size_t operator()(intergdb::common::Query const & q) const
        {
            return q.getHashCode();
        }
    };
}

