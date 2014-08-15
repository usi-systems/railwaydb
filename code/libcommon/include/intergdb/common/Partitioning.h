#pragma once

#include <intergdb/common/Partition.h>

#include <unordered_set>

namespace intergdb { namespace common
{

class Partition;

class Partitioning
{
public:
  Partitioning() {}
  void addPartition(Partition * partition) { partitions_.insert(partition); }
  void removePartition(Partition * partition) { partitions_.erase(partition); }
  std::unordered_set<Partition *> const & getPartitions() const { return partitions_; }
private:
  std::unordered_set<Partition *> partitions_;
};

} }
