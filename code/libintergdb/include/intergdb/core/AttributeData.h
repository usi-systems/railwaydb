#ifndef INTERGDB_ATTRIBUTEDATA_H
#define INTERGDB_ATTRIBUTEDATA_H

#include <string>
#include <vector>
#include <iostream> 
#include <sstream>     
#include <boost/variant.hpp>


namespace intergdb { namespace core
{
    typedef boost::variant<int64_t, double, std::string> vType;

    class Schema;

    class AttributeData
    {
        friend class Schema;
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
       os << data.toString();
       return os;
    }

} } /* namespace */

#endif /* INTERGDB_ATTRIBUTEDATA_H */
