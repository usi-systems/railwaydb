#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Query.h>

#include <string>
#include <assert.h>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;

char const * Attribute::dataTypeStrings_[] =
    { "INT64", "DOUBLE", "STRING", "UNDEFINED" };

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
    for (Attribute const * attribute : attributes_)
        ss << "\t" << attribute->toString() << endl;
    ss << "Queries:" << endl;
    for (QuerySummary const & query : queries_)
        ss  << "\t" << query.toString()
            << ", frequency: "
            << to_string(getFrequency(query)) << endl;
    ss << "Counts:" << endl;
    for (auto it : counts_)
        ss  << "\t" << it.first.toString()
            << ", count: "
            << it.second;
    ss << endl;
    return ss.str();
}

double QueryWorkload::getFrequency(QuerySummary const & s) const
{
    auto countIt = counts_.find(s);
    assert (countIt != counts_.end());
    return countIt->second / totalQueries_;
}

void QueryWorkload::setFrequency(QuerySummary const & s, double f)
{
    totalQueries_ = 1;
    counts_.emplace(s, f);
}

void QueryWorkload::addQuery(Query const & q)
{
    totalQueries_++;
    auto search = summaries_.find(q);
    if(search != summaries_.end()) {
        auto countIt = counts_.find(search->second);
        countIt->second++;
        counts_.emplace(search->second, countIt->second);
    } else {
        std::vector<Attribute const *> attributes;
        for (auto name : q.getAttributeNames()) {
            auto it = nameToAttribute_.find(name);
            assert(it != nameToAttribute_.end());
            attributes.push_back(it->second);
        }
        QuerySummary summary(attributes);
        queries_.push_back(summary);
        summaries_.emplace(q, summary);
        counts_.emplace(summary, 1.0);
    }
}



