#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <boost/variant.hpp>

namespace intergdb { namespace core
{
    class Schema;
    class NetworkByteBuffer;

    class AttributeData
    {
    public:
        friend class Schema;
        typedef boost::variant<int64_t, double, std::string> Type;

        AttributeData& setAttribute(std::string const & attributeName,
                                    Type value);

        AttributeData& setAttribute(int attributeIndex, Type value);

        AttributeData & setAttributes(AttributeData const & other);

        Type const & getAttribute(int attributeIndex) const;

        Type const & getAttribute(std::string const & attributeName) const;

        std::string const & getStringAttribute(int attributeIndex) const
        {
            return boost::get<std::string>(getAttribute(attributeIndex));
        }

        std::string const & getStringAttribute(std::string const & attributeName) const
        {
            return boost::get<std::string>(getAttribute(attributeName));
        }

        double const & getDoubleAttribute(int attributeIndex) const
        {
            return boost::get<double>(getAttribute(attributeIndex));
        }

        double const & getDoubleAttribute(
            std::string const & attributeName) const
        {
            return boost::get<double>(getAttribute(attributeName));
        }

        int64_t const & getIntAttribute(int attributeIndex) const
        {
            return boost::get<int64_t>(getAttribute(attributeIndex));
        }

        int64_t const & getIntAttribute(
            std::string const & attributeName) const
        {
            return boost::get<int64_t>(getAttribute(attributeName));
        }

        std::unordered_map<int,Type> const & getFields() const
        {
            return fields_;
        }

        bool operator==(AttributeData const & other);

        Schema const & getSchema() const { return schema_; }

        std::string toString() const;

        double getFieldSize(int index);

    private:
        AttributeData(Schema const & schema);

        AttributeData(Schema const & schema,
                      std::unordered_set<std::string> const & attributes);

        std::unordered_map<int,Type> fields_;
        Schema const & schema_;
    };

    inline std::ostream& operator<<(std::ostream &os,
                                    AttributeData const& data)
    {
       return os << data.toString();
    }

    NetworkByteBuffer & operator << (NetworkByteBuffer & sbuf,
                                     AttributeData const & val);
    NetworkByteBuffer & operator >> (NetworkByteBuffer & sbuf,
                                     AttributeData & data);

} } /* namespace */
