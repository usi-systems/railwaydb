#pragma once

#include <string>
#include <vector>
#include <iostream>    
#include <unordered_map>

#include <intergdb/common/Attribute.h>

using namespace intergdb::common;

namespace intergdb { namespace core
{
    class AttributeData;
    
    class Schema
    {
        friend class AttributeData;
    public:
        static char const * typeToString(int enumVal)
        {
            return dataTypesStrings[enumVal];
        }
        Schema() {}
        Schema(std::unordered_map<std::string, Attribute::DataType> nameAndType);
        static Schema & empty()
        {
            static Schema emptySchema;  
            return emptySchema;
        }
        AttributeData * newAttributeData() const;
        Schema & addAttribute(std::string const & name, Attribute::DataType type);
        std::vector<Attribute> const & getAttributes() const { return attributes_; }

        int getIndex(std::string const & name) const { return nameToIndex_.find(name)->second; } 
        int numAttributes() const { return attributes_.size(); }
        std::string toString() const;
    private:
        static char const * dataTypesStrings[];   
        std::vector<Attribute> attributes_;            
        std::unordered_map<std::string, int> nameToIndex_; /* map from name to index */
        double typeToSize(Attribute::DataType type);
    };
} } /* namespace */

