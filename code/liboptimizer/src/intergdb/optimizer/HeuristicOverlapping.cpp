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

Partitioning HeuristicOverlapping::solve(QueryWorkload const & workload, double storageThreshold) 
{
  Partitioning partitioning;
  for (Query const & query : workload.getQueries()) {
    Partition partition;
    for (Attribute const * attrb : query.getAttributes()) 
      partition.addAttribute(attrb);
    partitioning.addPartition(partition);
  }
  Cost costModel;
  while (costModel.getStorageOverhead(partitioning, workload) > storageThreshold) {
    double oldCost = costModel.getIOCost(partitioning, workload);
    auto const & partitions = partitioning.getPartitions();
    MinCostSolution<pair<int, int>> minCostSolution;
    for (int i=0, iu=partitions.size(); i<iu-1; ++i) {
      for (int j=i+1; j<iu; ++j) {
        Partition lhsP = partitions[i]; // copy
        Partition rhsP = partitions[j]; // copy
        double oldSize = costModel.getPartitionSize(lhsP) + costModel.getPartitionSize(rhsP);
        int newPIndex = partitioning.mergePartitions(i, j);
        double newCost = costModel.getIOCost(partitioning, workload);
        Partition const & newP = partitions[newPIndex];
        double newSize = costModel.getPartitionSize(newP);
        double cost = (newCost-oldCost) / (oldSize-newSize); 
        minCostSolution.push(make_pair(i, j), cost);
        partitioning.splitPartition(newPIndex, lhsP, rhsP);
      }
    }
    pair<int, int> partitionIndices = minCostSolution.getBestSolution();
    partitioning.mergePartitions(partitionIndices.first, partitionIndices.second);
  }
  return partitioning;
}
