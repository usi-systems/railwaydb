#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Query.h>

#include <string>
#include <assert.h>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;

char const * Attribute::dataTypeStrings_[] = { "INT64", "DOUBLE", "STRING", "UNDEFINED" };

string Attribute::toString() const
{     
  return "(name: " + name_ + ", index: " + to_string(index_) + ")";
}

string QuerySummary::toString() const
{ 
  if (attributes_.size()==0)
    return "[attributes: ]";
  string str = "[attributes: " + to_string(attributes_[0]->getIndex());
  for (size_t i=1, iu=attributes_.size(); i<iu; ++i) 
      str += "," + to_string(attributes_[i]->getIndex());
  str += "]";
  return str;
}

string QueryWorkload::toString() const
{ 
    std::stringstream ss;

    ss << "Attributes:" << endl;
    for (Attribute const & attribute : attributes_) {
        ss << "\t" << attribute.toString() << endl;
    }
    ss << "Queries:" << endl;
    for (QuerySummary const & query : queries_) {  
        ss  << "\t" << query.toString() 
            << ", frequency: " 
            << to_string(getFrequency(query)) << endl;
    }
    ss << "Counts:" << endl;
    for (auto it : counts_) {
        ss  << "\t" << it.first.toString()
            << ", count: "
            << it.second;
            
    }
    ss << endl;
  return ss.str();


}


double 
QueryWorkload::getFrequency(QuerySummary s) const
{
    auto countIt = counts_.find(s);
    if (countIt == counts_.end()) {
        assert(false);
    }
    return countIt->second / totalQueries_;   
}

void 
QueryWorkload::setFrequency(QuerySummary s, double f) 
{
    totalQueries_ = 1;
    counts_.emplace(std::pair<QuerySummary,double>(s, f));
}

 

void QueryWorkload::addQuery(Query q) 
{
    totalQueries_++;
    auto search = summaries_.find(q);
    if(search != summaries_.end()) {        
        auto countIt = counts_.find(search->second);
        countIt->second++;
        counts_.emplace(std::pair<QuerySummary,double>(search->second, countIt->second));
    } else {        
        std::vector<Attribute const *> attributes;
        for (auto name : q.getAttributeNames()) {
            auto it = nameToAttribute_.find(name);
            if(it == nameToAttribute_.end()) {        
                assert(false);
            }
            attributes.push_back(&it->second);            
        }
        QuerySummary summary(attributes);
        summaries_.emplace(std::pair<Query,QuerySummary>(q, summary));
        counts_.emplace(std::pair<QuerySummary,double>(summary, 1.0));
    }

}



