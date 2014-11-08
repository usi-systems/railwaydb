#include <intergdb/core/EdgeData.h>
#include <intergdb/core/Schema.h>

using namespace intergdb::core;

intergdb::core::EdgeData & EdgeData::setAttribute(std::string attributeName, vType value) 
{  
    fields_[schema_.getIndex(attributeName)] = value; return *this; 
}

intergdb::core::EdgeData & EdgeData::setAttribute(int attributeIndex, vType value) { 
    fields_[attributeIndex] = value; return *this; 
}

std::string EdgeData::toString() const 
{ 
    std::stringstream ss;
    for (auto a : fields_) {
        ss << a;
    }
    return ss.str(); 
}

bool EdgeData::operator==(const EdgeData* other) 
{             
    if (fields_.size() != other->fields_.size()) return false;    
    for(auto&& t : zip(fields_, other->fields_))      
        if (!(t.get<0>() == t.get<1>())) return false;
    return true; 
}




