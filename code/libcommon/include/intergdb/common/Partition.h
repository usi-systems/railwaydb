#pragma once

#include <intergdb/common/Attribute.h>

#include <sparsehash/dense_hash_set>

namespace intergdb { namespace common
{

class Attribute;

class Partition
{
public:
  Partition()
  {
    attributes_.set_empty_key(nullptr);
    attributes_.set_deleted_key(reinterpret_cast<Attribute const *>(-1));
  }
  void clearAttributes() { attributes_.clear(); }
  void addAttribute(Attribute const * attribute) { attributes_.insert(attribute); }
  void removeAttribute(Attribute const * attribute) { attributes_.erase(attribute); }
  int numAttributes() { return attributes_.size(); }
  bool operator==(Partition const & other) const;
  google::dense_hash_set<Attribute const *> const & getAttributes() const { return attributes_; }
  std::string toString() const;
private:
  google::dense_hash_set<Attribute const *> attributes_;
};

} }
