#pragma once

#include <string>
#include <vector>
#include <iostream>    
#include <unordered_set>
#include <unordered_map>

#include <intergdb/common/Attribute.h>

namespace intergdb { namespace core
{ 
    enum class DataType { INT64=0, DOUBLE, STRING, UNDEFINED };

    class AttributeData;
    
    class Schema
    {
        friend class AttributeData;
    public:
        Schema() {}
        Schema(std::vector<std::pair<std::string, DataType>> const & namesAndTypes);
        AttributeData * newAttributeData() const;
        AttributeData * newAttributeData(std::unordered_set<std::string> const & attributes) const;
        std::vector<common::Attribute> const & getAttributes() const { return attributes_; }
        int getIndex(std::string const & name) const { return nameToIndex_.find(name)->second; } 
        std::string toString() const;
    private:
        std::vector<common::Attribute> attributes_;            
        std::unordered_map<std::string, int> nameToIndex_; /* map from name to index */
        double typeToSize(DataType type);
    };
} } /* namespace */

