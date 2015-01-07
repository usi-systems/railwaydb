#pragma once

#include <intergdb/core/Types.h>
#include <intergdb/core/NeighborList.h>
#include <intergdb/core/EdgeFIFO.h>
#include <intergdb/core/ExpirationMap.h>
#include <intergdb/common/SchemaStats.h>

#include <deque>
#include <memory>
#include <unordered_map>
#include <cassert>

namespace intergdb { namespace core
{
    class AttributeData;
    class Conf;

    class InMemoryGraph
    {
    public:
        InMemoryGraph(Conf const & conf, HistoricalGraph * hisg, SchemaStats & stats);
        void addEdge(VertexId from, VertexId to, Timestamp ts, std::shared_ptr<AttributeData> data);
        void flush();
        BlockStats const & getBlockStats() const { return expm_.getBlockStats(); }
    private:
        std::shared_ptr<AttributeData> removeEdge(UEdge const & edge);
    private:
        EdgeFIFO vfifo_;
        ExpirationMap expm_;
        std::unordered_map<VertexId, NeighborList> neigLists_;
        SchemaStats & stats_;
    };

} } /* namespace */

