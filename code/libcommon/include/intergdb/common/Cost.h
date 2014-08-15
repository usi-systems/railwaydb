#pragma once

#include <intergdb/common/Partitioning.h>
#include <intergdb/common/QueryWorkload.h>

namespace intergdb { namespace common
{

class Cost
{
public:
    Cost() {}
    std::vector<Partition *> getUsedPartitionsOverlapping(
      Partitioning const & partitioning, std::vector<Attribute> const & attributes, 
      Query const & query);
    std::vector<Partition *> getUsedPartitionsNonOverlapping(
      Partitioning const & partitioning, std::vector<Attribute> const & attributes, 
      Query const & query);
    double getIOCost(Partitioning const & partitioning, QueryWorkload const & workload) ;
    double getStorageOverhead(Partitioning const & partitioning, QueryWorkload const &workload);
};

} }
