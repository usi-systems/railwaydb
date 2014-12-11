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
    std::cout << "QueryWorkload::addQuery " << q.toString() << std::endl;

    // We want to look at the QuerySummaries, and see if 
    // there is one with for this particular query

    totalQueries_++;

    // We want a mapping from Query -> QuerySummary
    auto search = summaries_.find(q);
    if(search != summaries_.end()) {        
        std::cout << "QueryWorkload::addQuery summary found" << std::endl;   


    } else {        
        std::cout << "QueryWorkload::addQuery summary not found" << std::endl;   

        std::vector<Attribute const *> attributes;
        for (auto name : q.getAttributeNames()) {
            std::cout << "QueryWorkload::addQuery " << name << std::endl;
            auto it = nameToAttribute_.find(name);
            if(it == nameToAttribute_.end()) {        
                // assert(false);
            }
            attributes.push_back(&it->second);
        }

        QuerySummary summary(attributes, 1.0);
        summaries_[q] = summary;
    }

}



