#ifndef INTERGDB_SCHEMA_H
#define INTERGDB_SCHEMA_H

#include <string>
#include <vector>
#include <iostream> 
#include <sstream>     
#include <unordered_map>


namespace intergdb { namespace core
{
    class EdgeData;
    
    class Schema
    {
        friend class EdgeData;
    public:
        enum DataType { INT64, DOUBLE, STRING };
        typedef std::pair<std::string, DataType> AttributeType;
        static char const * dataTypesStrings[];        
        static char const * typeToString(int enumVal)
        {
            return dataTypesStrings[enumVal];
        }

        Schema() {}
        // TODO: this is a special empty schema so that we can 
        // iterate over references
        static Schema & empty()
        {
            static Schema emptySchema;  
            return emptySchema;
        }

        EdgeData * newEdgeData();
        Schema & addAttribute(std::string const& name, DataType type);
        std::vector<AttributeType> & getAttributes() { return attributes_; }
        int getIndex(std::string const& name) { return nameToIndex_[name]; } 
        std::string toString() const 
        { 
            std::stringstream ss;   
            for (auto a : attributes_) 
                ss << a.first << ":" << typeToString(a.second) << " ";        
            return ss.str(); 
        }   
    private:
        std::vector<AttributeType> attributes_;            /* list of name/type pairs */
        std::unordered_map<std::string, int> nameToIndex_; /* map from name to index */
    };
} } /* namespace */

#endif /* INTERGDB_SCHEMA_H */
