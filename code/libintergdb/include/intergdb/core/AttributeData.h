#pragma once

#include <string>
#include <vector>
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
        typedef boost::variant<int64_t, double, std::string> vType;
    private:
        AttributeData(Schema const & schema, int size) : schema_(schema) { fields_.resize(size); }
    public:
        AttributeData& setAttribute(std::string const& attributeName, vType value);
        AttributeData& setAttribute(int attributeIndex, vType value);
        vType getAttribute(int attributeIndex) const { return fields_[attributeIndex]; };
        std::string toString() const;
        bool operator==(AttributeData const& other); 
        Schema const & getSchema() const { return schema_; }
    private:
        std::vector<vType> fields_;
        Schema const & schema_;
    };
   
    inline std::ostream& operator<<(std::ostream &os, AttributeData const& data)
    {      
       return os << data.toString();
    }

    NetworkByteBuffer & operator << (NetworkByteBuffer & sbuf, AttributeData const & val);
    NetworkByteBuffer & operator >> (NetworkByteBuffer & sbuf, AttributeData & data);

} } /* namespace */


