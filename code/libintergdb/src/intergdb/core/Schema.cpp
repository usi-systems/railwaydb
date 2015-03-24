#include <intergdb/core/Schema.h>

#include <intergdb/common/Attribute.h>
#include <intergdb/core/AttributeData.h>

#include <iostream>
#include <sstream>

using namespace std;
using namespace intergdb::common;
using namespace intergdb::core;

Schema::Schema(vector<pair<string, DataType>> const & namesAndTypes)
{
    size_t index = 0;
    for (auto const & nameAndType : namesAndTypes) {
        Attribute attribute(index, nameAndType.first,
            static_cast<Attribute::DataType>(nameAndType.second));
        attributes_.push_back(std::move(attribute));
        nameToIndex_.emplace(nameAndType.first, attributes_.size()-1);
        index++;
    }
}

AttributeData * Schema::newAttributeData() const
{
    return new AttributeData(*this);
}

AttributeData * Schema::newAttributeData(
    unordered_set<string> const & attributes) const
{
    return new AttributeData(*this, attributes);
}

std::string Schema::toString() const
{
    std::stringstream ss;
    for (auto a : attributes_)
        ss << a.toString() << " ";
    return ss.str();
}
