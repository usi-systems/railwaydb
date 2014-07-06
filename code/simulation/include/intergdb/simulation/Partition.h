#pragma once

#include <intergdb/simulation/Attribute.h>

#include <unordered_set>

namespace intergdb { namespace simulation
{

class Attribute;

class Partition
{
public:
  Partition() {}
  void addAttribute(Attribute * attribute) { attributes_.add(attribute); }
  void removeAttribute(Attribute * attribute) { attributes_.erase(attribute); }
  std::unordered_set<Attribute *> const & getAttributes() const { return attributes_; }
private:
  std::unordered_set<Attribute *> attributes_;
};

} }