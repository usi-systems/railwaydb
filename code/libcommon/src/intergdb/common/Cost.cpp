#include <intergdb/common/Cost.h>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;

double Cost::io(Partitioning * partitioning, QueryWorkload * workload) 
{
    return 0;
}

double Cost::storage(Partitioning * partitioning, QueryWorkload * workload)
{
    return 0;
}
