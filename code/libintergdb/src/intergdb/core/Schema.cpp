
#include <intergdb/core/Schema.h>
#include <intergdb/core/AttributeData.h>
#include <intergdb/common/Attribute.h>

#include <iostream> 
#include <sstream> 

using namespace intergdb::core;
 
Schema::Schema(std::unordered_map<std::string, common::Attribute::DataType> nameAndType) 
{

    size_t index = 0;
    for (auto p : nameAndType) {
        Attribute attribute(index, typeToSize(p.second), p.first, p.second);
        attributes_.emplace_back(attribute);
        index++;
    }
}

AttributeData * Schema::newAttributeData() const
{ 
    return new AttributeData(*this, attributes_.size()); 
}

Schema & Schema::addAttribute(std::string const& name, common::Attribute::DataType type) 
{    
    Attribute attribute(attributes_.size(), typeToSize(type), name, type);
    attributes_.emplace_back(attribute);
    nameToIndex_.insert({{name, attributes_.size()-1}});
    return *this; 
}

double Schema::typeToSize(common::Attribute::DataType type) 
{
    switch (type) {
    case Attribute::INT64: 
        return sizeof(int64_t);
    case Attribute::DOUBLE: 
        return sizeof(double);
    case Attribute::STRING:
        // TODO: This should be fixed to keep an average
        std::cerr << "Schema::typeToSize warning. String type does not have the proper size." << std::endl;
        return 128;
    }
    assert(false);
    return(0.0);
}


std::string Schema::toString() const 
{ 
    std::stringstream ss;   
    for (auto a : attributes_) 
        ss << a.toString() << " ";        
    return ss.str(); 
}   
