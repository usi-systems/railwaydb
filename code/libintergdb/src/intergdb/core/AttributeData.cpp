#include <intergdb/core/AttributeData.h>

#include <intergdb/core/Schema.h>
#include <intergdb/core/NetworkByteBuffer.h>
#include <intergdb/common/Attribute.h>

using namespace std;
using namespace intergdb::common;
using namespace intergdb::core;


AttributeData::Type getTypeValue(Attribute::DataType type)
{
    switch (type) {
    case Attribute::INT64:
        return AttributeData::Type(int64_t{});
    case Attribute::DOUBLE:
        return AttributeData::Type(double{});
    case Attribute::STRING:
        return AttributeData::Type(string{});
    default:
        assert("!cannot happen");
    } 
    return AttributeData::Type();
}

AttributeData::AttributeData(Schema const & schema) 
    : schema_(schema) 
{ 
    for (Attribute const & attrb : schema_.getAttributes()) 
        fields_.emplace(attrb.getIndex(), getTypeValue(attrb.getType()));
}

AttributeData::AttributeData(Schema const & schema, unordered_set<string> const & attributes) 
    : schema_(schema) 
{ 
    for(string const & attrb : attributes) {
        auto idx = schema.getIndex(attrb);
        auto const & type = schema.getAttributes().at(idx).getType();
        fields_.emplace(idx, getTypeValue(type));
    }
}

AttributeData & AttributeData::setAttribute(string const & attributeName, Type value) 
{  
    fields_[schema_.getIndex(attributeName)] = value; 
    return *this; 
}

AttributeData & AttributeData::setAttribute(int attributeIndex, Type value) 
{ 
    fields_[attributeIndex] = value; 
    return *this; 
}

AttributeData::Type const & AttributeData::getAttribute(string const & attributeName) const 
{ 
    return fields_.find(schema_.getIndex(attributeName))->second;
}

AttributeData::Type const & AttributeData::getAttribute(int attributeIndex) const 
{ 
    return fields_.find(attributeIndex)->second; 
}

std::string AttributeData::toString() const 
{ 
    std::stringstream ss;
    for (auto const & idxValuePair : fields_) {
        int idx = idxValuePair.first;
        AttributeData::Type const & value = idxValuePair.second;
        Attribute const & attrb = schema_.getAttributes().at(idx);
        switch (attrb.getType()) {
        case Attribute::INT64:
        {
            ss << boost::get<int64_t>(value);
            break;
        }
        case Attribute::DOUBLE:
        {
            ss << boost::get<double>(value);
            break;
        }
        case Attribute::STRING:
        {
            ss << boost::get<std::string>(value);
            break;
        }
        default:
            assert(false);
        } 
    }
    return ss.str(); 
}

bool AttributeData::operator==(AttributeData const& other) 
{             
    if (fields_.size() != other.fields_.size()) 
        return false;
    for (auto idxValuePair : fields_) {
        int idx = idxValuePair.first;
        AttributeData::Type const & value = idxValuePair.second;
        auto iter = other.fields_.find(idx);
        if (iter==other.fields_.end())
            return false;
        if (!(value==iter->second))
            return false;
    }
    return true; 
}

namespace intergdb { namespace core
{

    NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf, AttributeData const & data)
    {
        Schema const & schema = data.getSchema();
        for (auto const & idxValuePair : data.getFields()) {
            int idx = idxValuePair.first;
            Attribute const & attrb = schema.getAttributes().at(idx);
            AttributeData::Type const & value = idxValuePair.second;
            switch (attrb.getType()) {
            case Attribute::INT64:
            {
                sbuf << boost::get<int64_t>(value);
                break;
            }
            case Attribute::DOUBLE:
            {
                sbuf << boost::get<double>(value);
                break;
            }
            case Attribute::STRING:
            {
                sbuf << boost::get<std::string>(value);
                break;
            }
            default:
                assert(!"cannot happen");
            }
        }
        return sbuf;

    }

    NetworkByteBuffer & operator >> (NetworkByteBuffer & sbuf, AttributeData & data)
    {      
        Schema const & schema = data.getSchema();
        for (auto const & idxValuePair : data.getFields()) {
            int idx = idxValuePair.first;
            Attribute const & attrb = schema.getAttributes().at(idx);
            switch (attrb.getType()) {
            case Attribute::INT64:
            {
                int64_t dataInt64;
                sbuf >> dataInt64;
                data.setAttribute(idx, dataInt64);
                break;
            }
            case Attribute::DOUBLE:
            {
                double dataDouble;
                sbuf >> dataDouble;
                data.setAttribute(idx, dataDouble);
                break;
            }
            case Attribute::STRING:
            {
                std::string dataString;
                sbuf >> dataString;
                data.setAttribute(idx, dataString);
                break;
            }
            default:
                assert(!"cannot happen");
            } 
        }
        return sbuf;
    }

} }
