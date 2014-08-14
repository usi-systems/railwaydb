#pragma once

#include <intergdb/common/Attribute.h>

#include <unordered_set>

namespace intergdb { namespace common
{

class Attribute;

class Partition
{
public:
  Partition() {}
  void addAttribute(Attribute * attribute) { attributes_.insert(attribute); }
  void removeAttribute(Attribute * attribute) { attributes_.erase(attribute); }
  std::unordered_set<Attribute *> const & getAttributes() const { return attributes_; }
private:
  std::unordered_set<Attribute *> attributes_;
};

} }
