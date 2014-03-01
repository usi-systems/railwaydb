#ifndef INTERGDB_FOCUSEDINTERVALQUERYINDEX_H
#define INTERGDB_FOCUSEDINTERVALQUERYINDEX_H

#include <intergdb/core/Block.h>
#include <intergdb/core/BlockManager.h>
#include <intergdb/core/Conf.h>
#include <intergdb/core/Exceptions.h>
#include <intergdb/core/Types.h>
#include <intergdb/core/NetworkByteBuffer.h>
#include <leveldb/db.h>

#include <memory>
#include <tr1/memory>

namespace intergdb { namespace core
{
    template <class EdgeData>
    class FocusedIntervalQueryIndex
    {
    public:
        class Iterator
        {
        public:
            Iterator(leveldb::DB * db, VertexId vertex, Timestamp start, Timestamp end);
            bool isValid();
            void next();
            Timestamp getEndTime() const { return currentEnd_; }
            VertexId getVertex() const { return currentVertex_; }
            Timestamp getStartTime() const { return currentStart_; }
            BlockId getBlock() const { return currentBlock_; }
            Timestamp getRangeStartTime() const { return start_; }
            Timestamp getRangeEndTime() const { return end_; }
        private:
            void readCurrents();
        private:
            VertexId vertex_;
            Timestamp start_;
            Timestamp end_;
            // currents
            Timestamp currentStart_;
            Timestamp currentEnd_;
            VertexId currentVertex_;
            BlockId currentBlock_;
            std::auto_ptr<leveldb::Iterator> dbIter_;
        };
        FocusedIntervalQueryIndex(Conf const & conf);
        void indexBlock(Block<EdgeData> const & block);
        std::tr1::shared_ptr<Iterator> query(VertexId vertex, Timestamp start, Timestamp end);
    private:
        std::auto_ptr<leveldb::DB> db_;
    };

    #define FOCUSED_INTERVAL_QUERY_INDEX_NAME "fiq_index"

    template <class EdgeData>
    FocusedIntervalQueryIndex<EdgeData>::FocusedIntervalQueryIndex(Conf const & conf)
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

    #undef FOCUSED_INTERVAL_QUERY_INDEX_NAME

    template <class EdgeData>
    FocusedIntervalQueryIndex<EdgeData>::Iterator::Iterator(leveldb::DB * db,
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

    template <class EdgeData>
    bool FocusedIntervalQueryIndex<EdgeData>::FocusedIntervalQueryIndex::Iterator::isValid()
    {
        return dbIter_->Valid() && currentVertex_==vertex_ && currentStart_ < end_;
    }

    template <class EdgeData>
    void FocusedIntervalQueryIndex<EdgeData>::Iterator::next()
    {
        assert(isValid());
        dbIter_->Next();
        if (dbIter_->Valid())
            readCurrents();
    }

    template <class EdgeData>
    void FocusedIntervalQueryIndex<EdgeData>::Iterator::readCurrents()
    {
        assert(dbIter_->Valid());
        leveldb::Slice keyStr = dbIter_->key();
        leveldb::Slice valueStr = dbIter_->value();
        NetworkByteBuffer keyBuffer(reinterpret_cast<unsigned char *>(const_cast<char *>(keyStr.data())), keyStr.size());
        NetworkByteBuffer valueBuffer(reinterpret_cast<unsigned char *>(const_cast<char *>(valueStr.data())), valueStr.size());
        keyBuffer >> currentVertex_;
        keyBuffer >> currentEnd_;
        valueBuffer >> currentBlock_;
        valueBuffer >> currentStart_;
    }

    template <class EdgeData>
    std::tr1::shared_ptr<typename FocusedIntervalQueryIndex<EdgeData>::Iterator> FocusedIntervalQueryIndex<EdgeData>::
        query(VertexId vertex, Timestamp start, Timestamp end)
    {
        return std::tr1::shared_ptr<FocusedIntervalQueryIndex::Iterator>(new Iterator(db_.get(), vertex, start, end));
    }

    template <class EdgeData>
    void FocusedIntervalQueryIndex<EdgeData>::indexBlock(Block<EdgeData> const & block)
    {
        auto const & nlists = block.getNeighborLists();
        for (auto  it=nlists.begin(); it!=nlists.end(); ++it) {
            auto & nlist = it->second;
            VertexId head = nlist.headVertex();
            Timestamp start = nlist.getOldestEdge().getTime();
            Timestamp end = nlist.getNewestEdge().getTime();
            std::pair<VertexId, Timestamp> key(head, end);
            std::pair<BlockId, Timestamp> data(block.id(), start);

            NetworkByteBuffer keyBuf(sizeof(key));
            keyBuf << key;
            leveldb::Slice keySlice(reinterpret_cast<char *>(keyBuf.getPtr()), keyBuf.getSerializedDataSize());
            NetworkByteBuffer dataBuf(sizeof(data));
            dataBuf << data;
            leveldb::Slice dataSlice(reinterpret_cast<char *>(dataBuf.getPtr()), dataBuf.getSerializedDataSize());
            leveldb::Status status = db_->Put(leveldb::WriteOptions(), keySlice, dataSlice);
            if (!status.ok())
                throw std::runtime_error(status.ToString());
        }
    }
} } /* namespace */


#endif /* INTERGDB_FOCUSEDINTERVALQUERYINDEX_H */
