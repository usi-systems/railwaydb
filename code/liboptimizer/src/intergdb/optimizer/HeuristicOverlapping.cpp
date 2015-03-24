#include <intergdb/optimizer/HeuristicOverlapping.h>

#include <intergdb/common/Cost.h>
#include <intergdb/common/SystemConstants.h>
#include <intergdb/optimizer/MinCostSolution.h>

#include <memory>
#include <iostream>
#include <assert.h>

using namespace std;
using namespace intergdb::common;
using namespace intergdb::optimizer;

Partitioning HeuristicOverlapping::solve(
  QueryWorkload const & workload,
  double storageThreshold,
  SchemaStats const & stats)
{
  Partitioning partitioning;
  auto const & allAttributes = workload.getAttributes();
  unordered_set<Attribute const *> missingAttributes;
  for (auto const * attrb : allAttributes)
    missingAttributes.insert(attrb);
  for (QuerySummary const & query : workload.getQuerySummaries()) {
    Partition partition;
    for (Attribute const * attrb : query.getAttributes()) {
      partition.addAttribute(attrb);
      if (missingAttributes.count(attrb)>0)
        missingAttributes.erase(attrb);
    }
    partitioning.addPartition(partition);
  }
  if (missingAttributes.size()>0) {
    Partition partition;
    for (Attribute const * attrb : missingAttributes)
      partition.addAttribute(attrb);
    partitioning.addPartition(partition);
  }
  Cost costModel(stats);
  while (costModel.getStorageOverhead(partitioning, workload) > storageThreshold) {
    double oldCost = costModel.getIOCost(partitioning, workload);
    auto const & partitions = partitioning.getPartitions();
    MinCostSolution<pair<int, int>> minCostSolution;
    //Partitioning partitioningClone = partitioning;
    for (int i=0, iu=partitions.size(); i<iu-1; ++i) {
      for (int j=i+1; j<iu; ++j) {
        Partition lhsP = partitions[i]; // copy
        Partition rhsP = partitions[j]; // copy
        double oldSize = costModel.getPartitionSize(lhsP) + costModel.getPartitionSize(rhsP);
        partitioning.mergePartitions(i, j);
        double newCost = costModel.getIOCost(partitioning, workload);
        Partition const & newP = partitions[i];
        double newSize = costModel.getPartitionSize(newP);
        double cost = (newCost-oldCost) / (oldSize-newSize);
        minCostSolution.push(make_pair(i, j), cost);
        partitioning.splitPartition(i, j, lhsP, rhsP);
        // assert(partitioning==partitioningClone);
      }
    }
    pair<int, int> partitionIndices = minCostSolution.getBestSolution();
    partitioning.mergePartitions(partitionIndices.first, partitionIndices.second);
  }

  return partitioning;
}
