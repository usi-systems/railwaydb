#include <intergdb/common/Cost.h>
#include <intergdb/common/SystemConstants.h>

#include <iostream>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;

vector<Partition const *> Cost::getUsedPartitions(vector<Partition> const & partitions, 
  std::unordered_set<Attribute const *> const & attributes, Query const & query)
{
  // attributes we have covered so far
  unordered_set<Attribute const *> selectedAttributes; 
  // attributes we have yet to cover
  unordered_set<Attribute const *> remainingAttributes; 
  for (Attribute const * attribute : attributes)
    remainingAttributes.insert(attribute);
  // attributes that appear in queries that we have to cover
  unordered_set<Attribute const *> effectiveAttributes; 
  for (Attribute const * attribute : query.getAttributes())
    if (attributes.count(attribute)>0)
      effectiveAttributes.insert(attribute);

  vector<Partition const *> usedPartitions;
  unordered_set<Partition const *> unusedPartitions;
  for (Partition const & partition: partitions) 
    unusedPartitions.insert(&partition);
  
  while (remainingAttributes.size()!=0) {
    Partition const * bestPartition = nullptr;
    double bestPartitionScore = -1.0;
    for (Partition const * partition : unusedPartitions) {
      double partitionScore = 0.0;
      double partitionSize = getPartitionSize(*partition);
      for (Attribute const * attribute : partition->getAttributes()) {
        if (selectedAttributes.count(attribute)>0) 
          continue;
        if (effectiveAttributes.count(attribute)==0)
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

double Cost::getIOCost(Partitioning const & partitioning, QueryWorkload const & workload) 
{
  unordered_set<Attribute const *> attributes;
  for (Attribute const & attrb : workload.getAttributes())
    attributes.insert(&attrb);
  return getIOCost(partitioning.getPartitions(), workload, attributes);
}

double Cost::getIOCost(vector<Partition> const & partitions, QueryWorkload const & workload, 
      std::unordered_set<Attribute const *> const & attributes)
{
  double totalIOCost = 0.0;
  auto const & queries = workload.getQueries();
  for (Query const & query : queries) {
    double partitionIOCost = 0.0;
    vector<Partition const *> usedPartitions = getUsedPartitions(partitions, attributes, query);
    for (Partition const * partition : usedPartitions) 
      partitionIOCost += getPartitionSize(*partition);
    totalIOCost += query.getFrequency() * partitionIOCost;
  }
  return totalIOCost;
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

double Cost::getStorageOverhead(Partitioning const & partitioning, QueryWorkload const & workload)
{
  auto const & attributes = workload.getAttributes();
  Partition wholePartition;
  for (Attribute const & attribute : attributes)
    wholePartition.addAttribute(&attribute);
  double blockSize = getPartitionSize(wholePartition);
  double totalPartitionSize = 0.0;
  for (Partition const & partition : partitioning.getPartitions())
    totalPartitionSize += getPartitionSize(partition);
  return totalPartitionSize/blockSize - 1.0;
}

