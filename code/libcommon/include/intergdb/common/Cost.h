#pragma once

#include <intergdb/common/Partitioning.h>
#include <intergdb/common/QueryWorkload.h>

namespace intergdb { namespace common
{

class Cost
{
public:
    Cost() {}
    double io(Partitioning * partitioning, QueryWorkload * workload) ;
    double storage(Partitioning * partitioning, QueryWorkload * workload);
};

} }
