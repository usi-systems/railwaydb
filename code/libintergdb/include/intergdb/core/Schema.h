#ifndef INTERGDB_SCHEMA_H
#define INTERGDB_SCHEMA_H

#include <string>
#include <list>
#include <iostream> 
#include <sstream>     
#include <unordered_map>

#include <intergdb/core/EdgeData.h>

namespace intergdb { namespace core
{

    typedef std::pair<std::string, int> attribute;

    class Schema
    {
        friend class EdgeData;
    public:
        Schema() { }
        EdgeData * newEdgeData() const { return new EdgeData(attributes_.size()); }
        Schema & addAttribute(std::string name, int type) { 
            //attributes_.push_back(std::make_pair(name, type))
            return *this; 
        }
        std::string toString() const { return "Schema"; }   
    private:
        std::list<attribute> attributes_; /* list of name/type pairs */
        std::unordered_map<std::string,int> nameToIndex_; /* map from name to index */
  
    };


} } /* namespace */

#endif /* INTERGDB_SCHEMA_H */
