#pragma once

#include <intergdb/common/Attribute.h>
#include <intergdb/common/QuerySummary.h>
#include <intergdb/core/Query.h>

#include <vector>

namespace intergdb { namespace common
{
  class QueryWorkload
  {

  public:
    QueryWorkload() {}
    QueryWorkload(std::vector<Attribute> const & attributes)
      : attributes_(attributes) {}
    QueryWorkload(std::vector<Attribute> const & attributes, 
        std::vector<QuerySummary> const & queries) 
      : attributes_(attributes), queries_(queries) {} 
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

    void addQuery(core::Query q);

  private:
    std::vector<Attribute> attributes_;
    std::vector<QuerySummary> queries_;
  };


} } /* namespace */

