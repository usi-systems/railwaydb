#pragma once

#include <vector>

namespace intergdb { namespace common
{
  class Attribute;
  class QuerySummary
  {
  public:
    QuerySummary() {}
    QuerySummary(std::vector<Attribute const *> const & attributes, double frequency) 
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
} }
