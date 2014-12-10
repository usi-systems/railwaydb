#include <intergdb/core/AttributeData.h>

#include <intergdb/core/Schema.h>
#include <intergdb/core/NetworkByteBuffer.h>
#include <intergdb/common/Attribute.h>

using namespace intergdb::core;

intergdb::core::AttributeData& AttributeData::setAttribute(std::string const& attributeName, Type value) 
{  
    fields_[schema_.getIndex(attributeName)] = value; 
    return *this; 
}

intergdb::core::AttributeData& AttributeData::setAttribute(int attributeIndex, Type value) { 
    fields_[attributeIndex] = value; 
    return *this; 
}

std::string AttributeData::toString() const 
{ 
    std::stringstream ss;
    int i = 0;
    for (auto a : schema_.getAttributes()) {
        switch (a.getType()) {
        case Attribute::INT64:
        {
            ss << boost::get<int64_t>(fields_[i]);
            break;
        }
        case Attribute::DOUBLE:
        {
            ss << boost::get<double>(fields_[i]);
            break;
        }
        case Attribute::STRING:
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

namespace intergdb { namespace core
{

    NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf, AttributeData const & val)
    {
        Schema const & schema = val.getSchema();
        int i = 0;
        for (auto const & a : schema.getAttributes()) {
            switch (a.getType()) {
            case Attribute::INT64:
            {
                sbuf << boost::get<int64_t>(val.getAttribute(i));
                break;
            }
            case Attribute::DOUBLE:
            {
                sbuf << boost::get<double>(val.getAttribute(i));
                break;
            }
            case Attribute::STRING:
            {
                sbuf << boost::get<std::string>(val.getAttribute(i));
                break;
            }
            default:
                assert(false);
            }
            i++; 
        }
        return sbuf;

    }

    NetworkByteBuffer & operator >> (NetworkByteBuffer & sbuf, AttributeData & data)
    {      
        Schema const & schema = data.getSchema();
        int i = 0;
        for (auto const & a : schema.getAttributes()) {
            switch (a.getType()) {
            case Attribute::INT64:
            {
                int64_t dataInt64;
                sbuf >> dataInt64;
                data.setAttribute(i, dataInt64);
                break;
            }
            case Attribute::DOUBLE:
            {
                double dataDouble;
                sbuf >> dataDouble;
                data.setAttribute(i, dataDouble);
                break;
            }
            case Attribute::STRING:
            {
                std::string dataString;
                sbuf >> dataString;
                data.setAttribute(i, dataString);
                break;
            }
            default:
                assert(false);
            } 
            i++;
        }
        return sbuf;
    }

} }
