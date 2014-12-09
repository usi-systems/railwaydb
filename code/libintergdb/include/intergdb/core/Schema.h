#pragma once

#include <string>
#include <vector>
#include <iostream>    
#include <unordered_map>


namespace intergdb { namespace core
{
    class AttributeData;
    
    class Schema
    {
        friend class AttributeData;
    public:
        enum DataType { INT64, DOUBLE, STRING };
        typedef std::pair<std::string, DataType> AttributeType;             
        static char const * typeToString(int enumVal)
        {
            return dataTypesStrings[enumVal];
        }
        Schema() {}
        Schema(std::unordered_map<std::string, Schema::DataType> nameAndType);
        static Schema & empty()
        {
            static Schema emptySchema;  
            return emptySchema;
        }
        AttributeData * newAttributeData() const;
        Schema & addAttribute(std::string const & name, DataType type);
        std::vector<AttributeType> const & getAttributes() const { return attributes_; }
        int getIndex(std::string const & name) const { return nameToIndex_.find(name)->second; } 
        int numAttributes() const { return attributes_.size(); }
        std::string toString() const;
    private:
        static char const * dataTypesStrings[];   
        std::vector<AttributeType> attributes_;            /* list of name/type pairs */
        std::unordered_map<std::string, int> nameToIndex_; /* map from name to index */
    };
} } /* namespace */

