
#include <assert.h>
#include <iostream>

#include <HeuristicNonOverlapping.h>

#include <intergdb/common/Cost.h>
#include <intergdb/common/SystemConstants.h>

using namespace std;
using namespace intergdb::common;
using namespace intergdb::optimizer;

template <typename SolutionT>
class MinCostSolution
{
public:
  MinCostSolution() : 
    inited_(false), 
    minCost_(std::numeric_limits<double>::max()) 
  {}
  void push(SolutionT solution, double cost)  
  {
    if (!inited_ || cost <= minCost_) { 
      minCost_ = cost;
      bestSolution_ = solution;
      inited_ = true;
    }
  } 
  double getMinCost() const 
  {
    return minCost_;
  }
  SolutionT const & getBestSolution() const
  {
    return bestSolution_;
  }
  bool isInited() const 
  {
    return inited_;
  }
private:
  bool inited_;
  SolutionT bestSolution_;
  double minCost_;
};


Partitioning HeuristicNonOverlapping::solve(QueryWorkload const & workload, double storageThreshold) 
{
  Cost costModel;
  MinCostSolution<Partitioning> minCostPart;
  for (int numPartitions=1; numPartitions<=workload.getAttributes().size(); ++numPartitions) {
    Partitioning partitioning = solve(workload, storageThreshold, numPartitions);
    double overhead = costModel.getStorageOverhead(partitioning, workload);
    if (overhead > storageThreshold)
      continue;
    double cost = costModel.getIOCost(partitioning, workload);
    minCostPart.push(partitioning, cost);
  }
  assert(minCostPart.isInited());
  return minCostPart.getBestSolution();
}

Partitioning HeuristicNonOverlapping::solve(QueryWorkload const & workload, double storageThreshold, int numPartitions) 
{
  Partitioning part;
  return part;
}



