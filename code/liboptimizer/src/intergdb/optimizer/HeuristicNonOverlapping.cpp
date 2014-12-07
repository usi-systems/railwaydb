#include <intergdb/optimizer/HeuristicNonOverlapping.h>

#include <intergdb/common/Cost.h>
#include <intergdb/common/SystemConstants.h>
#include <intergdb/optimizer/MinCostSolution.h>

#include <iostream>
#include <unordered_map>
#include <assert.h>

using namespace std;
using namespace intergdb::common;
using namespace intergdb::optimizer;

Partitioning HeuristicNonOverlapping::solve(QueryWorkload const & workload, double storageThreshold) 
{
  Cost costModel;
  MinCostSolution<Partitioning> minCostPart;
  for (int numPartitions=1; numPartitions<=workload.getAttributes().size(); ++numPartitions) {
    Partitioning partitioning = solve(workload, storageThreshold, numPartitions);
    double overhead = costModel.getStorageOverhead(partitioning, workload);
    if (overhead > storageThreshold)
      break;
    double cost = costModel.getIOCost(partitioning, workload);
    minCostPart.push(partitioning, cost);
  }
  assert(minCostPart.isInited());
  return minCostPart.getBestSolution();
}

Partitioning HeuristicNonOverlapping::solve(QueryWorkload const & workload, double storageThreshold, int numPartitions) 
{
  // Initialize partitions
  vector<Partition> partitions;
  for (int i=0; i<numPartitions; ++i)
    partitions.emplace_back(Partition());

  // Perform greedy placement
  //  - Order attributes in decreasing order of frequency
  vector<Attribute const *> attributes;
  unordered_map<Attribute const *, int> attrbFreq;
  for (Attribute const & attrb : workload.getAttributes()) {
    attrbFreq[&attrb] = 0;
    attributes.push_back(&attrb);
  }
  for (QuerySummary const & query : workload.getQueries())
    for (Attribute const * attrb : query.getAttributes())
      ++(attrbFreq[attrb]);
  std::sort(begin(attributes), end(attributes), 
    [&](Attribute const * lhs, Attribute const * rhs) 
  {
    return attrbFreq[lhs] > attrbFreq[rhs];
  });
  // - In decreasing order of attribute frequency
  Cost costModel;
  unordered_set<Attribute const *> usedAttributes;
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



