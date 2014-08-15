#include <intergdb/common/Cost.h>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;

vector<Partition *> Cost::getUsedPartitions(Partitioning const & partitioning, 
  std::vector<Attribute> const & attributes, Query const & query)
{
  vector<Partition *> usedPartitions;
  // TODO
  return usedPartitions;
}

double Cost::getIOCost(Partitioning const & partitioning, QueryWorkload const & workload) 
{
  return 0.0;
}

double Cost::getStorageOverhead(Partitioning const & partitioning, QueryWorkload const & workload)
{
  return 0.0;
}