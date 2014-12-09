
#include <intergdb/core/Schema.h>
#include <intergdb/core/AttributeData.h>

#include <iostream> 
#include <sstream> 

using namespace intergdb::core;

char const * Schema::dataTypesStrings[] = { "INT64", "DOUBLE", "STRING" };
 
Schema::Schema(std::unordered_map<std::string, Schema::DataType> nameAndType) 
{
    for (auto p : nameAndType) {
        attributes_.emplace_back(p.first, p.second);
    }
}

AttributeData * Schema::newAttributeData() const
{ 
    return new AttributeData(*this, attributes_.size()); 
}

Schema & Schema::addAttribute(std::string const& name, DataType type) 
{ 
    attributes_.emplace_back(name, type);
    nameToIndex_.insert({{name, attributes_.size()-1}});
    return *this; 
}


std::string Schema::toString() const 
{ 
    std::stringstream ss;   
    for (auto a : attributes_) 
        ss << a.first << ":" << typeToString(a.second) << " ";        
    return ss.str(); 
}   