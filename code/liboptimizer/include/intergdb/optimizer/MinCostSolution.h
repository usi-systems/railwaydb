#pragma once

namespace intergdb { namespace optimizer {

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

} }