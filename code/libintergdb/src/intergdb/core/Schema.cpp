
#include <intergdb/core/Schema.h>
#include <intergdb/core/EdgeData.h>

using namespace intergdb::core;

EdgeData * Schema::newEdgeData()  
{ 
    return new EdgeData(*this, attributes_.size()); 
}

Schema & Schema::addAttribute(std::string name, DataType type) 
{ 
    attributes_.push_back(std::make_pair(name, type));
    return *this; 
}

