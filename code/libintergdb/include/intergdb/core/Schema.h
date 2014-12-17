#pragma once

#include <string>
#include <vector>
#include <iostream>    
#include <unordered_map>
#include <unordered_set>

#include <intergdb/common/Attribute.h>

namespace intergdb { namespace core
{
    class AttributeData;
    
    class Schema
    {
        friend class AttributeData;
    public:
        Schema() {}
        Schema(std::unordered_map<std::string, common::Attribute::DataType> const & nameAndType);
        AttributeData * newAttributeData() const;
        AttributeData * newAttributeData(std::unordered_set<std::string> const & attributes) const;
        std::vector<common::Attribute> const & getAttributes() const { return attributes_; }
        int getIndex(std::string const & name) const { return nameToIndex_.find(name)->second; } 
        std::string toString() const;
    private:
        std::vector<common::Attribute> attributes_;            
        std::unordered_map<std::string, int> nameToIndex_; /* map from name to index */
        double typeToSize(common::Attribute::DataType type);
    };
} } /* namespace */

