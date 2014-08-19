
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
  Solution const & getBestSolution() const
  {
    return bestSolution;
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
  MinCostSolution<Partitioning> minCostPart;
  for (int numPartitions=1; j<workload.getAttributes().size(); ++numPartitions) {
    Partitioning partitioning = solve(workload, storageThreshold, numPartitions);
    double overhead = Cost::getStorageOverhead(partitioning, workload);
    if (overhead > storageThreshold)
      continue;
    double cost = Cost::getIOCost(partitioning);
    minCostPart.push(parting, cost);
  }
  assert(minCostPart.isInited());
  return minCostPart.getBestSolution();
}





