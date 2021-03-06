#include <intergdb/core/AttributeData.h>

#include <intergdb/common/Attribute.h>
#include <intergdb/core/Exceptions.h>
#include <intergdb/core/Helper.h>
#include <intergdb/core/NetworkByteBuffer.h>
#include <intergdb/core/Schema.h>

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
    auto it = fields_.find(schema_.getIndex(attributeName));
    if (it==fields_.end())
        throw attribute_not_found_exception(attributeName);
    it->second = value;
    return *this;
}

AttributeData & AttributeData::setAttribute(int attributeIndex, Type value)
{
    fields_[attributeIndex] = value;
    return *this;
}

AttributeData & AttributeData::setAttributes(AttributeData const & other)
{
    for (auto const & indexValuePair : other.fields_) {
        auto iter = fields_.find(indexValuePair.first);
        if (iter != fields_.end())
            iter->second = indexValuePair.second;
    }
    return *this;
}

AttributeData::Type const & AttributeData::getAttribute(string const & attributeName) const
{
    auto it = fields_.find(schema_.getIndex(attributeName));
    if (it==fields_.end())
        throw attribute_not_found_exception(attributeName);
    return it->second;
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

double AttributeData::getFieldSize(int index)
{
    switch (schema_.getAttributes().at(index).getType()) {
    case Attribute::INT64:
    {
        return getSerializedSizeOf(getIntAttribute(index));
    }
    case Attribute::DOUBLE:
    {
        return getSerializedSizeOf(getDoubleAttribute(index));
    }
    case Attribute::STRING:
    {
        return getSerializedSizeOf(getStringAttribute(index));
    }
    default:
        assert(false);
    }
    return 0;
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

    NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf,
                                   AttributeData const & data)
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

    NetworkByteBuffer & operator >> (NetworkByteBuffer & sbuf,
                                     AttributeData & data)
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
