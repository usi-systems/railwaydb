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
    private:
        AttributeData(Schema const & schema);
        AttributeData(Schema const & schema, std::unordered_set<std::string> const & attributes);
    public:
        AttributeData& setAttribute(std::string const & attributeName, Type value);
        AttributeData& setAttribute(int attributeIndex, Type value);
        Type const & getAttribute(int attributeIndex) const;
        Type const & getAttribute(std::string const & attributeName) const;
        std::unordered_map<int,Type> const & getFields() const { return fields_; }
        bool operator==(AttributeData const & other); 
        Schema const & getSchema() const { return schema_; }
        std::string toString() const;
    private:
        std::unordered_map<int,Type> fields_;
        Schema const & schema_;
    };
   
    inline std::ostream& operator<<(std::ostream &os, AttributeData const& data)
    {      
       return os << data.toString();
    }

    NetworkByteBuffer & operator << (NetworkByteBuffer & sbuf, AttributeData const & val);
    NetworkByteBuffer & operator >> (NetworkByteBuffer & sbuf, AttributeData & data);

} } /* namespace */


