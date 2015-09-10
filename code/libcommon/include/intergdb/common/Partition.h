#pragma once

#include <intergdb/common/Attribute.h>

#include <unordered_set>

namespace intergdb { namespace common
{

class Attribute;

class Partition
{
public:
  Partition()
  {}
  void clearAttributes() { attributes_.clear(); }
  void addAttribute(Attribute const * attribute) { attributes_.insert(attribute); }
  void removeAttribute(Attribute const * attribute) { attributes_.erase(attribute); }
  int numAttributes() { return attributes_.size(); }
  bool operator==(Partition const & other) const;
  std::unordered_set<Attribute const *> const & getAttributes() const { return attributes_; }
  std::string toString() const;
private:
  std::unordered_set<Attribute const *> attributes_;
};

} }
