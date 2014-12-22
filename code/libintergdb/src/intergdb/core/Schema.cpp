
#include <intergdb/core/Schema.h>
#include <intergdb/core/AttributeData.h>
#include <intergdb/common/Attribute.h>

#include <iostream> 
#include <sstream> 

using namespace std;
using namespace intergdb::common;
using namespace intergdb::core;
 
Schema::Schema(vector<pair<string, DataType>> const & namesAndTypes) 
{
    size_t index = 0;
    for (auto const & nameAndType : namesAndTypes) {
        Attribute attribute(index, typeToSize(nameAndType.second), nameAndType.first, 
            static_cast<Attribute::DataType>(nameAndType.second));
        attributes_.push_back(std::move(attribute));
        nameToIndex_.emplace(nameAndType.first, attributes_.size()-1);
        index++;
    }
}

AttributeData * Schema::newAttributeData() const
{ 
    return new AttributeData(*this); 
}

AttributeData * Schema::newAttributeData(unordered_set<string> const & attributes) const
{ 
    return new AttributeData(*this, attributes); 
}

double Schema::typeToSize(DataType type) 
{
    switch (type) {
    case DataType::INT64: 
        return sizeof(int64_t);
    case DataType::DOUBLE: 
        return sizeof(double);
    case DataType::STRING:
        // TODO: This should be fixed to keep an average
        std::cerr << "Schema::typeToSize warning. String type does not have the proper size." << std::endl;
        return 128;
    case DataType::UNDEFINED:
        assert(false);
    }
    assert(false);
    return(0.0);
}


std::string Schema::toString() const 
{ 
    std::stringstream ss;   
    for (auto a : attributes_) 
        ss << a.toString() << " ";        
    return ss.str(); 
}   
