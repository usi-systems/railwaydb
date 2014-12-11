#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Query.h>

#include <string>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;

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

    // We want to look at the QuerySummaries, and see if 
    // there is one with for this particular query

    totalQueries_++;

    // We want a mapping from Query -> QuerySummary
    auto search = summaries_.find(q);
    if(search != summaries_.end()) {        
        std::cout << "QueryWorkload::addQuery summary found" << std::endl;   


    } else {        
        std::cout << "QueryWorkload::addQuery summary not found" << std::endl;   
        // TODO: We need to create a QuerySummary of the Query
        // We therefore need to create a std::vector<Attribute const *>
        // We have a std::vector<std::string> that contains the attribute names
        // To map string name to type (for size), we need access to the schema
        
        // consult nameToAttribute_

        //QuerySummary summary(, 1.0);
        // summaries_[q] = summary;
    }

}



