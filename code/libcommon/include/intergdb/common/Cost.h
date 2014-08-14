#pragma once

#include <intergdb/common/Partitioning.h>
#include <intergdb/common/QueryWorkload.h>

namespace intergdb { namespace common
{

class Partitioning;
class QueryWorkload;

class Cost
{
public:
    Cost() {}
    int io(Partitioning partitioning, QueryWorkload workload) ;
    int storage(Partitioning partitioning, QueryWorkload workload);
};

} }
