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
  std::vector<Partition> const & getPartitions() const { return partitions_; }
private:
  std::vector<Partition> partitions_;
};

} }
