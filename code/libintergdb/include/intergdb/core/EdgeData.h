#ifndef INTERGDB_EDGEDATA_H
#define INTERGDB_EDGEDATA_H

#include <string>
#include <vector>
#include <iostream> 
#include <sstream>     
#include <boost/variant.hpp>
//#include <boost/iterator/zip_iterator.hpp>
//#include <boost/range.hpp>


namespace intergdb { namespace core
{
    typedef boost::variant<int64_t, double, std::string> vType;

    class Schema;

    class EdgeData
    {
        friend class Schema;
    private:
        EdgeData(Schema const & schema, int size) : schema_(schema) { fields_.resize(size); }
    public:
        EdgeData& setAttribute(std::string const& attributeName, vType value);
        EdgeData& setAttribute(int attributeIndex, vType value);
        vType getAttribute(int attributeIndex) const { return fields_[attributeIndex]; };
        std::string toString() const;
        bool operator==(EdgeData const& other); 
        Schema const & getSchema() const { return schema_; }
    private:
        std::vector<vType> fields_;
        Schema const & schema_;
    };
   
    inline std::ostream& operator<<(std::ostream &os, EdgeData const& data)
    {      
       os << data.toString();
       return os;
    }

} } /* namespace */

#endif /* INTERGDB_EDGE_H */
