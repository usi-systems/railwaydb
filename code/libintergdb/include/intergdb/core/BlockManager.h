#pragma once

#include <intergdb/core/Block.h>
#include <leveldb/db.h>

#include <memory>
#include <list>
#include <unordered_map>

namespace intergdb { namespace core
{
    class Schema;
    class Conf;

    class BlockManager
    {
    private:
        struct BlockAndIdIter
        {
            BlockAndIdIter() {};
            Block block;
            std::list<BlockId>::iterator iter;
        };
    public:
        BlockManager(Conf const & conf);
        Block const & getBlock(BlockId id);
        void addBlock(Block & data);
        double getHitRatio() { return hitCount_/static_cast<double>(reqCount_); }
        size_t getNumIOReads() const { return nIOReads_; }
        size_t getNumIOWrites() const { return nIOWrites_; }
    private:
        void findNextBlockId();
    private:
        size_t nIOReads_;
        size_t nIOWrites_;
        BlockId nextBlockId_;
        size_t reqCount_;
        size_t hitCount_;
        size_t blockBufferSize_;
        std::list<BlockId> lruList_;
        Schema const & edgeSchema_;
        std::unordered_map<BlockId, BlockAndIdIter> cache_;
        std::auto_ptr<leveldb::DB> db_;
    };

} } /* namespace */

