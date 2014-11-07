#ifndef INTERGDB_SCHEMA_H
#define INTERGDB_SCHEMA_H

#include <string>
#include <list>
#include <iostream> 
#include <sstream>     
#include <unordered_map>


namespace intergdb { namespace core
{

    class EdgeData;

    enum DataType { INT64, DOUBLE, STRING } ;
    static const char * dataTypesStrings[] = { "INT64", "DOUBLE", "STRING" };

    const char * typeToString(int enumVal)
    {
        return dataTypesStrings[enumVal];
    }

    typedef std::pair<std::string, DataType> attribute;

    class Schema
    {
        friend class EdgeData;
    public:
        Schema() { }
        EdgeData * newEdgeData() const ;
        Schema & addAttribute(std::string name, DataType type) ;
        std::string toString() const { return "Schema"; }   
    private:
        std::list<attribute> attributes_;                  /* list of name/type pairs */
        std::unordered_map<std::string, int> nameToIndex_; /* map from name to index */
  
    };


} } /* namespace */

#endif /* INTERGDB_SCHEMA_H */
