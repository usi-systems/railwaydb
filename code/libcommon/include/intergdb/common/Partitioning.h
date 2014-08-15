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
  std::string toString() const;
private:
  std::vector<Partition> partitions_;
};

} }
