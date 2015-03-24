#include <intergdb/core/FocusedIntervalQueryIndex.h>

#include <intergdb/core/Block.h>
#include <intergdb/core/BlockManager.h>
#include <intergdb/core/Conf.h>
#include <intergdb/core/Exceptions.h>
#include <intergdb/core/NetworkByteBuffer.h>

using namespace std;
using namespace intergdb::core;

#define FOCUSED_INTERVAL_QUERY_INDEX_NAME "fiq_index"

FocusedIntervalQueryIndex::FocusedIntervalQueryIndex(Conf const & conf)
{
    leveldb::Options options;
    options.create_if_missing = true;
    options.max_open_files = 100;
    leveldb::DB * db;
    leveldb::Status status = leveldb::DB::Open(options,
            conf.getStorageDir()+"/"+FOCUSED_INTERVAL_QUERY_INDEX_NAME, &db);
    if (!status.ok())
        throw std::runtime_error(status.ToString());
    db_.reset(db);
}

FocusedIntervalQueryIndex::Iterator::Iterator(leveldb::DB * db,
        VertexId vertex, Timestamp start, Timestamp end)
    : vertex_(vertex), start_(start), end_(end)
{
    dbIter_.reset(db->NewIterator(leveldb::ReadOptions()));
    NetworkByteBuffer targetBuf(sizeof(VertexId)+sizeof(Timestamp));
    targetBuf << vertex;
    targetBuf << start;
    leveldb::Slice targetSlice(reinterpret_cast<char *>(targetBuf.getPtr()),
                               targetBuf.getSerializedDataSize());
    dbIter_->Seek(targetSlice);
    if (dbIter_->Valid())
        readCurrents();
}

bool FocusedIntervalQueryIndex::FocusedIntervalQueryIndex::Iterator::isValid()
{
    return dbIter_->Valid() && currentVertex_==vertex_ && currentStart_ < end_;
}

void FocusedIntervalQueryIndex::Iterator::next()
{
    assert(isValid());
    dbIter_->Next();
    if (dbIter_->Valid())
        readCurrents();
}

void FocusedIntervalQueryIndex::Iterator::readCurrents()
{
    assert(dbIter_->Valid());
    leveldb::Slice keyStr = dbIter_->key();
    leveldb::Slice valueStr = dbIter_->value();
    NetworkByteBuffer keyBuffer(reinterpret_cast<unsigned char *>(
        const_cast<char *>(keyStr.data())), keyStr.size());
    NetworkByteBuffer valueBuffer(reinterpret_cast<unsigned char *>(
        const_cast<char *>(valueStr.data())), valueStr.size());
    keyBuffer >> currentVertex_;
    keyBuffer >> currentEnd_;
    valueBuffer >> currentBlock_;
    valueBuffer >> currentStart_;
}

std::shared_ptr<typename FocusedIntervalQueryIndex::Iterator>
    FocusedIntervalQueryIndex::query(
        VertexId vertex, Timestamp start, Timestamp end)
{
    return std::shared_ptr<FocusedIntervalQueryIndex::Iterator>(
        new Iterator(db_.get(), vertex, start, end));
}

void FocusedIntervalQueryIndex::indexBlock(Block const & block)
{
    auto const & nlists = block.getNeighborLists();
    for (auto  it=nlists.begin(); it!=nlists.end(); ++it) {
        auto & nlist = it->second;
        VertexId head = nlist.headVertex();
        Timestamp start = nlist.getOldestEdge().getTime();
        Timestamp end = nlist.getNewestEdge().getTime();
        pair<VertexId, Timestamp> key(head, end);
        pair<BlockId, Timestamp> data(block.id(), start);
        NetworkByteBuffer keyBuf(sizeof(key));
        keyBuf << key;
        leveldb::Slice keySlice(reinterpret_cast<char *>(keyBuf.getPtr()),
                                keyBuf.getSerializedDataSize());
        NetworkByteBuffer dataBuf(sizeof(data));
        dataBuf << data;
        leveldb::Slice dataSlice(reinterpret_cast<char *>(dataBuf.getPtr()),
                                 dataBuf.getSerializedDataSize());
        leveldb::Status status = db_->Put(leveldb::WriteOptions(), keySlice,
                                          dataSlice);
        if (!status.ok())
            throw std::runtime_error(status.ToString());
    }
}
