#pragma once

#include <intergdb/common/Partition.h>

#include <vector>

namespace intergdb { namespace common
{

class Partitioning
{
public:
  Partitioning() {}
  void addPartition(Partition const & partition) { 
    partitions_.push_back(partition); 
  }
  int numPartitions() { return partitions_.size(); }
  std::vector<Partition> const & getPartitions() const { return partitions_; }
  int mergePartitions(int i, int j);
  std::pair<int, int> splitPartition(int i, Partition const & lhs, Partition const & rhs);
  std::string toString() const;
private:
  std::vector<Partition> partitions_;
};

} }
