#pragma once

#include <intergdb/core/AttributeData.h>
#include <intergdb/core/Types.h>

#include <leveldb/db.h>

#include <list>
#include <memory>

namespace intergdb { namespace core
{
    class Conf;
    class Schema;

    class VertexManager
    {
    private:
        struct VertexDataAndIdIter
        {
            std::shared_ptr<AttributeData> data;
            std::list<VertexId>::iterator iter;
        };

    public:
        VertexManager(Conf const & conf);

        void addVertex(VertexId id, AttributeData const & data);

        std::shared_ptr<AttributeData> getVertexData(VertexId id);

        double getHitRatio()
        {
            return hitCount_/static_cast<double>(reqCount_);
        }

    private:
        size_t reqCount_;
        size_t hitCount_;
        size_t vertexDataBufferSize_;
        std::list<VertexId> lruList_;
        std::unordered_map<VertexId, VertexDataAndIdIter> cache_;
        std::auto_ptr<leveldb::DB> db_;
        Schema const & vertexSchema_;
    };
} } /* namespace */
