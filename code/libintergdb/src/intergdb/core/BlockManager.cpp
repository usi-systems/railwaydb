#include <intergdb/core/BlockManager.h>

#include <intergdb/core/Conf.h>
#include <intergdb/core/Exceptions.h>
#include <intergdb/core/Types.h>
#include <intergdb/core/Block.h>
#include <intergdb/core/NetworkByteBuffer.h>

using namespace std;
using namespace intergdb::core;

#define BLOCK_DB_NAME "block_data"

BlockManager::BlockManager(
    Conf const & conf, PartitionIndex & partitionIndex, MetaDataManager & meta)
    : nIOReads_(0), nIOWrites_(0), reqCount_(0), hitCount_(0),
    blockBufferSize_(conf.blockBufferSize()),
    edgeSchema_(conf.getEdgeSchema()), partitionIndex_(partitionIndex),
    meta_(meta), nextBlockId_(meta_.getNextBlockId())
{
    leveldb::Options options;
    options.create_if_missing = true;
    options.max_open_files = 100;
    leveldb::DB * db;
    leveldb::Status status = leveldb::DB::Open(options,
            conf.getStorageDir()+"/"+BLOCK_DB_NAME, &db);
    if (!status.ok())
        throw std::runtime_error(status.ToString());
    db_.reset(db);
    nextBlockId_ = meta_.getNextBlockId();
    partitionIndex_.setBlockManager(this);
}

Block const & BlockManager::getBlock(BlockId id)
{
    reqCount_++;
    auto it = cache_.find(id);
    if (it!=cache_.end()) {
        hitCount_++;
        BlockAndIdIter & blockAndIter = it->second;
        if (blockAndIter.iter != lruList_.begin()) {
            lruList_.erase(blockAndIter.iter);
            lruList_.push_front(id);
            blockAndIter.iter = lruList_.begin();
        }
        assert(blockAndIter.block.id()==id);
        return blockAndIter.block;
    } else {
        NetworkByteBuffer keyBuf(sizeof(id));
        keyBuf << id;
        leveldb::Slice key(reinterpret_cast<char *>(keyBuf.getPtr()),
                           keyBuf.getSerializedDataSize());
        std::string value;
        leveldb::Status status = db_->Get(leveldb::ReadOptions(), key, &value);
        if (status.IsNotFound())
            throw block_not_found_exception(id);
        else if (!status.ok())
            throw std::runtime_error(status.ToString());
        NetworkByteBuffer dataBuf(
            reinterpret_cast<unsigned char *>(const_cast<char *>(
                value.data())), value.size());
        nIOReads_ += keyBuf.getSerializedDataSize() +
            dataBuf.getSerializedDataSize();
        BlockAndIdIter & blockAndIter =
            cache_.emplace(id, BlockAndIdIter()).first->second;
        blockAndIter.block.deserialize(dataBuf, edgeSchema_, partitionIndex_);
        lruList_.push_front(id);
        blockAndIter.iter = lruList_.begin();
        if (lruList_.size()>blockBufferSize_) {
            VertexId r = lruList_.back();
            lruList_.pop_back();
            // no need to write back, as this is an append only db
            cache_.erase(r);
        }
        assert(blockAndIter.block.id()==id);
        return blockAndIter.block;
    }
}

void BlockManager::addBlock(Block & block, bool setId/*=true*/)
{
    if (setId)
        block.id() = nextBlockId_++;
    NetworkByteBuffer keyBuf(sizeof(BlockId));
    keyBuf << block.id();
    leveldb::Slice key(reinterpret_cast<char *>(keyBuf.getPtr()),
                       keyBuf.getSerializedDataSize());
    NetworkByteBuffer dataBuf;
    block.serialize(dataBuf);
    leveldb::Slice dataSlice(reinterpret_cast<char *>(dataBuf.getPtr()),
                             dataBuf.getSerializedDataSize());
    leveldb::Status status = db_->Put(leveldb::WriteOptions(), key, dataSlice);
    if (!status.ok())
        throw std::runtime_error(status.ToString());
    nIOWrites_ += keyBuf.getSerializedDataSize() +
        dataBuf.getSerializedDataSize();
}

void BlockManager::updateBlock(Block const & block)
{
    Block & existingBlock = const_cast<Block &>(getBlock(block.id()));
    existingBlock = block;
    addBlock(existingBlock, false);
}

void BlockManager::removeBlock(BlockId blockId)
{
    NetworkByteBuffer keyBuf(sizeof(BlockId));
    keyBuf << blockId;
    leveldb::Slice key(reinterpret_cast<char *>(keyBuf.getPtr()),
                       keyBuf.getSerializedDataSize());
    leveldb::Status status = db_->Delete(leveldb::WriteOptions(), key);
    if (!status.ok())
        throw std::runtime_error(status.ToString());
    nIOWrites_ += keyBuf.getSerializedDataSize();
    if (cache_.count(blockId) > 0) {
        lruList_.erase(cache_[blockId].iter);
        cache_.erase(blockId);
    }
}
