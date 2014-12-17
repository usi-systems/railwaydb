#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Query.h>

#include <string>
#include <assert.h>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;

char const * Attribute::dataTypeStrings_[] = { "INT64", "DOUBLE", "STRING" };

string Attribute::toString() const
{ 
  return "(name: " + name_ + ", type: " + getTypeString(type_) +", index: " + to_string(index_) + ", size:" + to_string(size_) + ")";
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

// TODO (rjs): This is really inneficient. Need to change this so that the 
// workloads compute the frequencies when they are asked for it. 
void QueryWorkload::updateFrequencies() 
{
    for (auto it = summaries_.begin(); it != summaries_.end(); it++) {
        auto countIt = counts_.find(it->first);
        it->second.setFrequency(countIt->second/totalQueries_);
    }
}

void QueryWorkload::addQuery(Query q) 
{
    totalQueries_++;
    auto search = summaries_.find(q);
    if(search != summaries_.end()) {        
        auto countIt = counts_.find(q);
        countIt->second++;
        counts_.emplace(std::pair<Query,int>(q, countIt->second));
        updateFrequencies();
    } else {        
        std::vector<Attribute const *> attributes;
        for (auto name : q.getAttributeNames()) {
            auto it = nameToAttribute_.find(name);
            if(it == nameToAttribute_.end()) {        
                assert(false);
            }
            attributes.push_back(&it->second);
        }
        QuerySummary summary(attributes, 1.0);
        summaries_.emplace(std::pair<Query,QuerySummary>(q, summary));
        counts_.emplace(std::pair<Query,int>(q, 1));
        updateFrequencies();    
    }

}



