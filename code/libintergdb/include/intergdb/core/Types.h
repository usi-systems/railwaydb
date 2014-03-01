#ifndef INTERGDB_TYPES_H
#define INTERGDB_TYPES_H

#include <tr1/unordered_map>
#include <iostream>
#include <inttypes.h>

namespace intergdb { namespace core
{
    typedef uint64_t VertexId;
    typedef uint64_t BlockId;
    typedef double Timestamp; // in millisecs

} } /* namespace */

#endif /* INTERGDB_INMEM_INMEMORYGRAPH_H */
