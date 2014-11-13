
#include <intergdb/core/Schema.h>
#include <intergdb/core/EdgeData.h>

using namespace intergdb::core;

char const * Schema::dataTypesStrings[] = { "INT64", "DOUBLE", "STRING" };
 
EdgeData * Schema::newEdgeData() const
{ 
    return new EdgeData(*this, attributes_.size()); 
}

Schema & Schema::addAttribute(std::string const& name, DataType type) 
{ 
    attributes_.emplace_back(name, type);
    return *this; 
}

