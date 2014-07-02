#pragma once

#include <vector>

namespace intergdb { namespace simulation
{
  class Attribute
  {
  public:
    Attribute() {}
    Attribute(size_t index, double size) : index_(index), size_(size) {}
    size_t getIndex() const { return index_; }
    void setIndex(size_t index) { index_ = index; }
    double getSize() const { return size_; }
    void setSize(double size) { size_ = size; }
    std::string toString() const;
  private:
    size_t index_;
    double size_;
  };

  class Query
  {
  public:
    Query() {}
    Query(std::vector<Attribute const *> const & attributes, double frequency) 
      : attributes_(attributes), frequency_(frequency) {} 
    void addAttribute(Attribute const & attribute) { attributes_.push_back(&attribute); }
    void setAttributes(std::vector<Attribute const *> const & attributes) { attributes_ = attributes; }
    std::vector<Attribute const *> const & getAttributes() const { return attributes_; }
    void setFrequency(double frequency) { frequency_ = frequency; }
    double getFrequency() const { return frequency_; }
    std::string toString() const;
  private:
    std::vector<Attribute const *> attributes_;
    double frequency_;
  };

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

