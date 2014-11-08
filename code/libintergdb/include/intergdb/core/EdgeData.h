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
        EdgeData(Schema & schema, int size) : schema_(schema) { fields_.resize(size); }
    public:
        EdgeData& setAttribute(std::string const& attributeName, vType value);
        EdgeData& setAttribute(int attributeIndex, vType value);
        std::string toString() const;
        bool operator==(EdgeData const& other); 

    private:
        std::vector<vType> fields_;
        Schema & schema_;
        
        /*
        template <typename... T>
        auto zip(const T&... containers) -> boost::iterator_range<boost::zip_iterator<decltype(boost::make_tuple(std::begin(containers)...))>>
        {
            auto zip_begin = boost::make_zip_iterator(boost::make_tuple(std::begin(containers)...));
            auto zip_end = boost::make_zip_iterator(boost::make_tuple(std::end(containers)...));
            return boost::make_iterator_range(zip_begin, zip_end);
        }
        */
    };
   
    inline std::ostream& operator<<(std::ostream &os, EdgeData const& data)
    {      
       os << data.toString();
       return os;
    }

} } /* namespace */

#endif /* INTERGDB_EDGE_H */
