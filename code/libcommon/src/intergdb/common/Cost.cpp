#include <intergdb/common/Cost.h>
#include <intergdb/common/SystemConstants.h>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;

vector<Partition const *> Cost::getUsedPartitions(Partitioning const & partitioning, 
  std::vector<Attribute> const & attributes, Query const & query)
{
  unordered_set<Attribute const *> selectedAttributes;
  unordered_set<Attribute const *> remainingAttributes; 
  for (Attribute const & attribute : attributes)
    remainingAttributes.insert(&attribute);
  vector<Partition const *> usedPartitions;
  unordered_set<Partition const *> unusedPartitions;
  for (Partition const partition: partitioning.getPartitions())
    unusedPartitions.insert(&partition);
  unordered_set<Attribute const *> queryAttributes;
  for (Attribute const * attribute : query.getAttributes())
    queryAttributes.insert(attribute);
  while (selectedAttributes.size()!=attributes.size()) {
    Partition const * bestPartition = nullptr;
    double bestPartitionScore = -1.0;
    for (Partition const * partition : unusedPartitions) {
      double partitionScore = 0.0;
      double partitionSize = getPartitionSize(*partition);
      for (Attribute const * attribute : partition->getAttributes()) {
        if (selectedAttributes.count(attribute)>0) 
          continue;
        if (queryAttributes.count(attribute)==0)
          continue;
        partitionScore += (attribute->getSize() * 
          SystemConstants::numberOfEdgesInABlock) / partitionSize;
      }
      if (partitionScore > bestPartitionScore) {
        bestPartitionScore = partitionScore;
        bestPartition = partition;
      }
    }
    for(Attribute const * attribute : bestPartition->getAttributes()) {
      selectedAttributes.insert(attribute); 
      remainingAttributes.erase(attribute);
    }
    usedPartitions.push_back(bestPartition);
    unusedPartitions.erase(bestPartition);
  }
  return usedPartitions;
}

double Cost::getPartitionSize(Partition const & partition)
{
  double attributesSize = 0.0;
  for (Attribute const * attribute : partition.getAttributes())
    attributesSize += attribute->getSize();
  return 
    SystemConstants::numberOfEdgesInABlock * 
      ((SystemConstants::edgeIdSize + SystemConstants::timestampSize) + attributesSize)
     +
    SystemConstants::numberOfNeighborListsInABlock * 
      (SystemConstants::headVertexSize + SystemConstants::numEntriesSize);
}

double Cost::getIOCost(Partitioning const & partitioning, QueryWorkload const & workload) 
{
  return 0.0;
}

double Cost::getStorageOverhead(Partitioning const & partitioning, QueryWorkload const & workload)
{
  return 0.0;
}