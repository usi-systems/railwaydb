#pragma once

#include <intergdb/common/Attribute.h>
#include <intergdb/common/SchemaStats.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace intergdb { namespace core
{
    enum class DataType { INT64=0, DOUBLE, STRING, UNDEFINED };

    class AttributeData;

    class Schema
    {
    public:
        Schema()
        {}

        Schema(std::vector<std::pair<std::string, DataType>> const &
            namesAndTypes);
        AttributeData * newAttributeData() const;

        AttributeData * newAttributeData(
            std::unordered_set<std::string> const & attributes) const;

        std::vector<common::Attribute> const & getAttributes() const
        {
            return attributes_;
        }

        int getIndex(std::string const & name) const
        {
            return nameToIndex_.find(name)->second;
        }

        std::string toString() const;

    private:
        std::vector<common::Attribute> attributes_;
        // map from name to index
        std::unordered_map<std::string, int> nameToIndex_;
        friend class AttributeData;
    };
} } /* namespace */
