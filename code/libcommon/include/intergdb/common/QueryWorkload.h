#pragma once

#include <intergdb/common/Attribute.h>
#include <intergdb/common/Query.h>

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
        std::vector<Query> const & queries) 
      : attributes_(attributes), queries_(queries) {} 
    void addAttribute(Attribute const & attribute) { attributes_.push_back(attribute); }
    void setAttributes(std::vector<Attribute> const & attributes) { attributes_ = attributes; }
    std::vector<Attribute> const & getAttributes() const { return attributes_; }
    std::vector<Attribute> & getAttributes() { return attributes_; }
    void addQuery(Query const & query) { queries_.push_back(query); }
    void setQueries(std::vector<Query> const & queries) { queries_ = queries; }
    std::vector<Query> const & getQueries() const { return queries_; }
    std::vector<Query> & getQueries() { return queries_; }
    std::string toString() const;
  private:
    std::vector<Attribute> attributes_;
    std::vector<Query> queries_;
  };


} } /* namespace */

