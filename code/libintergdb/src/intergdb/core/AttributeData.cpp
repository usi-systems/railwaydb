#include <intergdb/core/AttributeData.h>
#include <intergdb/core/Schema.h>

using namespace intergdb::core;

intergdb::core::AttributeData& AttributeData::setAttribute(std::string const& attributeName, vType value) 
{  
    fields_[schema_.getIndex(attributeName)] = value; 
    return *this; 
}

intergdb::core::AttributeData& AttributeData::setAttribute(int attributeIndex, vType value) { 
    fields_[attributeIndex] = value; 
    return *this; 
}

std::string AttributeData::toString() const 
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

bool AttributeData::operator==(AttributeData const& other) 
{             
    if (fields_.size() != other.fields_.size()) 
        return false;
    for (size_t i=0, iu=fields_.size(); i<iu; ++i)
        if ( !(fields_[i]==other.fields_[i]) )
            return false;
    return true; 
}




