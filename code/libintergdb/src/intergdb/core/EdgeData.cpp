#include <intergdb/core/EdgeData.h>
#include <intergdb/core/Schema.h>

using namespace intergdb::core;

intergdb::core::EdgeData & EdgeData::setAttribute(std::string attributeName, vType value) 
{  
    int i = 0; fields_[i] = value; return *this; 
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

bool EdgeData::operator=(const EdgeData* rhs) 
{             
    if (fields_.size() != rhs->fields_.size()) return false;
/*
  for(auto&& t : zip(fields_, rhs->fields_))
  if (t.get<0>() != t.get<1>() ) return false;
*/
    return true; 
}


