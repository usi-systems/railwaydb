#include <intergdb/optimizer/HeuristicNonOverlapping.h>

#include <intergdb/common/Cost.h>
#include <intergdb/common/SystemConstants.h>
#include <intergdb/common/SchemaStats.h>

#include <intergdb/optimizer/MinCostSolution.h>

#include <iostream>
#include <sparsehash/dense_hash_map>
#include <assert.h>

using namespace std;
using namespace google;
using namespace intergdb::common;
using namespace intergdb::optimizer;

Partitioning HeuristicNonOverlapping::solve(QueryWorkload const & workload, double storageThreshold, SchemaStats const & stats)
{
  Cost costModel(stats);
  MinCostSolution<Partitioning> minCostPart;
  for (int numPartitions=1; numPartitions<=workload.getAttributes().size(); ++numPartitions) {
      Partitioning partitioning = solve(workload, storageThreshold, numPartitions, stats);
    double overhead = costModel.getStorageOverhead(partitioning, workload);
    if (overhead > storageThreshold)
      break;
    double cost = costModel.getIOCost(partitioning, workload);
    minCostPart.push(partitioning, cost);
  }
  assert(minCostPart.isInited());
  return minCostPart.getBestSolution();
}

Partitioning HeuristicNonOverlapping::solve(QueryWorkload const & workload, double storageThreshold, int numPartitions, SchemaStats const & stats)
{
  // Initialize partitions
  vector<Partition> partitions(numPartitions, Partition());

  // Perform greedy placement
  //  - Order attributes in decreasing order of frequency
  vector<Attribute const *> attributes(workload.getAttributes());
  dense_hash_map<Attribute const *, int> attrbFreq;
  attrbFreq.set_empty_key(nullptr);
  for (Attribute const * attrb : workload.getAttributes())
    attrbFreq[attrb] = 0;
  for (QuerySummary const & query : workload.getQuerySummaries())
    for (Attribute const * attrb : query.getAttributes())
      ++(attrbFreq[attrb]);
  std::sort(begin(attributes), end(attributes),
    [&](Attribute const * lhs, Attribute const * rhs)
  {
    return attrbFreq[lhs] > attrbFreq[rhs];
  });
  // - In decreasing order of attribute frequency
  Cost costModel(stats);
  dense_hash_set<Attribute const *> usedAttributes;
  usedAttributes.set_empty_key(nullptr);
  for (Attribute const * attrb : attributes) {
    usedAttributes.insert(attrb);
    MinCostSolution<Partition *> minCostPart;
    for (Partition & part : partitions) {
      part.addAttribute(attrb);
      double cost = costModel.getIOCost(partitions, workload, usedAttributes);
      minCostPart.push(&part, cost);
      part.removeAttribute(attrb);
    }
    Partition const * bestPart = minCostPart.getBestSolution();
    const_cast<Partition *>(bestPart)->addAttribute(attrb);
  }

  // Create a partitioning
  Partitioning partitioning;
  for (Partition const & partition : partitions)
    if (partition.getAttributes().size()>0)
      partitioning.addPartition(partition);
  return partitioning;
}



