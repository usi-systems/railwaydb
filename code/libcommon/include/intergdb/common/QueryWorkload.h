#pragma once

#include <intergdb/common/Attribute.h>
#include <intergdb/common/QuerySummary.h>
#include <intergdb/common/Query.h>
// #include <intergdb/common/Schema.h>


#include <vector>
#include <unordered_map>

namespace intergdb { namespace common
{
  class QueryWorkload
  {

  public:
    QueryWorkload() {}
    QueryWorkload(std::vector<Attribute> const & attributes)
        : attributes_(attributes),  totalQueries_(0) 
    {
        for (auto &attribute : attributes) 
        {
            nameToAttribute_[attribute.getName()] = attribute;
        }
    }

    QueryWorkload(std::vector<Attribute> const & attributes, 
        std::vector<QuerySummary> const & queries) 
        : attributes_(attributes), queries_(queries), totalQueries_(0) 
    {
        for (auto &attribute : attributes) 
        {
            nameToAttribute_[attribute.getName()] = attribute;
        }
    } 
    void addAttribute(Attribute const & attribute) { attributes_.push_back(attribute); }
    Attribute const & getAttribute(int index) const { return attributes_.at(index); }
    void setAttributes(std::vector<Attribute> const & attributes) { attributes_ = attributes; }
    std::vector<Attribute> const & getAttributes() const { return attributes_; }
    std::vector<Attribute> & getAttributes() { return attributes_; }
    void addQuerySummary(QuerySummary const & query) { queries_.push_back(query); }
    void setQuerySummaries(std::vector<QuerySummary> const & queries) { queries_ = queries; }
    std::vector<QuerySummary> const & getQuerySummaries() const { return queries_; }
    std::vector<QuerySummary> & getQuerySummaries() { return queries_; }
    std::string toString() const;
    void addQuery(Query q);
    double getFrequency(QuerySummary s) const;
    void setFrequency(QuerySummary s, double f);

  private:
    void updateFrequencies(); // TODO (rjs): get rid of this method
    std::vector<Attribute> attributes_;
    std::vector<QuerySummary> queries_;
    std::unordered_map<Query, QuerySummary> summaries_;
    std::unordered_map<QuerySummary, double> counts_;
    std::unordered_map<std::string, Attribute> nameToAttribute_;
    double totalQueries_;
  };


} } /* namespace */

