#include <intergdb/common/QueryWorkload.h>
#include <intergdb/core/Query.h>

#include <string>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;
using namespace intergdb::core;

string Attribute::toString() const
{ 
  return "(index: " + to_string(index_) + ", size:" + to_string(size_) + ")";
}

string QuerySummary::toString() const
{ 
  if (attributes_.size()==0)
    return "[attributes: ]";
  string str = "[attributes: " + to_string(attributes_[0]->getIndex());
  for (size_t i=1, iu=attributes_.size(); i<iu; ++i) 
    str += "," + to_string(attributes_[i]->getIndex());    
  str += ", frequency: " + to_string(frequency_) + "]";
  return str;
}

string QueryWorkload::toString() const
{ 
  string str = "Attributes:\n";
  for (Attribute const & attribute : attributes_) 
    str += "\t" + attribute.toString() + "\n";
  str += "Queries:\n";
  for (QuerySummary const & query : queries_) 
    str += "\t" + query.toString() + "\n";
  return str;
}


void QueryWorkload::addQuery(Query q) 
{
    std::cout << "QueryWorkload::addQuery" << std::endl;
}

