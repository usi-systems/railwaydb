#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/Partitioning.h>
#include <intergdb/common/SchemaStats.h>

namespace intergdb { namespace optimizer {

class Solver
{
public:
    virtual ~Solver() { }
    virtual std::string getClassName() = 0;
    virtual intergdb::common::Partitioning solve(intergdb::common::QueryWorkload const & workload, double storageThreshold, intergdb::common::SchemaStats const & stats) = 0;  
};

} }

