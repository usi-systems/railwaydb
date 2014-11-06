#ifndef INTERGDB_EDGEDATA_H
#define INTERGDB_EDGEDATA_H

#include <string>
#include <vector>
#include <iostream> 
#include <sstream>     
#include "boost/variant.hpp"
#include <boost/iterator/zip_iterator.hpp>
#include <boost/range.hpp>


namespace intergdb { namespace core
{

    typedef boost::variant<int64_t, double, std::string> vType;

    class EdgeData
    {
    public:
        EdgeData(int size) { fields_.reserve(size); }

        EdgeData & setAttribute(std::string attributeName, vType value) {  int i = 0; fields_[i] = value; return *this; }
        EdgeData & setAttribute(int attributeIndex, vType value) { fields_[attributeIndex] = value; return *this; }

        std::string toString() const { 
            std::stringstream ss;
            for (auto a : fields_) {
                ss << a;
            }
            return ss.str(); 
        }
        bool operator=(const EdgeData* rhs) {             
            if (fields_.size() != rhs->fields_.size()) return false;
/*
            for(auto&& t : zip(fields_, rhs->fields_))
                if (t.get<0>() != t.get<1>() ) return false;
*/
            return true; 
        }
    private:
        std::vector<vType> fields_;
        
        template <typename... T>
        auto zip(const T&... containers) -> boost::iterator_range<boost::zip_iterator<decltype(boost::make_tuple(std::begin(containers)...))>>
        {
            auto zip_begin = boost::make_zip_iterator(boost::make_tuple(std::begin(containers)...));
            auto zip_end = boost::make_zip_iterator(boost::make_tuple(std::end(containers)...));
            return boost::make_iterator_range(zip_begin, zip_end);
        }
    };

    std::ostream& operator<<( std::ostream &os, const EdgeData& data)
    {
        
        os << data.toString();
        return os;
    }

} } /* namespace */

#endif /* INTERGDB_EDGE_H */
