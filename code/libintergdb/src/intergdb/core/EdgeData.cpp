#include <intergdb/core/EdgeData.h>
#include <intergdb/core/Schema.h>

using namespace intergdb::core;

intergdb::core::EdgeData& EdgeData::setAttribute(std::string const& attributeName, vType value) 
{  
    fields_[schema_.getIndex(attributeName)] = value; 
    return *this; 
}

intergdb::core::EdgeData& EdgeData::setAttribute(int attributeIndex, vType value) { 
    fields_[attributeIndex] = value; 
    return *this; 
}

std::string EdgeData::toString() const 
{ 
    std::stringstream ss;
    int i = 0;
    for (auto a : schema_.getAttributes()) {
        switch (a.second) {
        case Schema::INT64:
        {
            ss << boost::get<int64_t>(fields_[i]);
            break;
        }
        case Schema::DOUBLE:
        {
            ss << boost::get<double>(fields_[i]);
            break;
        }
        case Schema::STRING:
        {
            ss << boost::get<std::string>(fields_[i]);
            break;
        }
        default:
            assert(false);
        } 
        i++;
    }
    return ss.str(); 
}

bool EdgeData::operator==(EdgeData const& other) 
{             
    if (fields_.size() != other.fields_.size()) 
        return false;
    for (size_t i=0, iu=fields_.size(); i<iu; ++i)
        if ( !(fields_[i]==other.fields_[i]) )
            return false;
    /*
    for(auto&& t : zip(fields_, other->fields_))      
        if (!(t.get<0>() == t.get<1>())) 
            return false;
    */
    return true; 
}




