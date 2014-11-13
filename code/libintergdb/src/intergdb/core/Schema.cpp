
#include <intergdb/core/Schema.h>
#include <intergdb/core/AttributeData.h>

using namespace intergdb::core;

char const * Schema::dataTypesStrings[] = { "INT64", "DOUBLE", "STRING" };
 
AttributeData * Schema::newAttributeData() const
{ 
    return new AttributeData(*this, attributes_.size()); 
}

Schema & Schema::addAttribute(std::string const& name, DataType type) 
{ 
    attributes_.emplace_back(name, type);
    return *this; 
}

