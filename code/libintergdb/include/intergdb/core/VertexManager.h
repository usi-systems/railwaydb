#ifndef INTERGDB_VERTEXMANAGER_H
#define INTERGDB_VERTEXMANAGER_H

#include <intergdb/core/Conf.h>
#include <intergdb/core/Exceptions.h>
#include <intergdb/core/Types.h>
#include <intergdb/core/NetworkByteBuffer.h>
#include <leveldb/db.h>

#include <memory>
#include <list>

namespace intergdb { namespace core
{
    class VertexManager
    {
    private:
        struct VertexDataAndIdIter
        {
            std::shared_ptr<AttributeData> data;
            std::list<VertexId>::iterator iter;
        };
    public:
        VertexManager(Conf const & conf, Schema const & vertexSchema);
        void addVertex(VertexId id, AttributeData * data);
        std::shared_ptr<AttributeData> getVertexData(VertexId id);
        double getHitRatio() { return hitCount_/static_cast<double>(reqCount_); }
    private:
        size_t reqCount_;
        size_t hitCount_;
        size_t vertexDataBufferSize_;
        std::list<VertexId> lruList_;
        std::unordered_map<VertexId, VertexDataAndIdIter> cache_;
        std::auto_ptr<leveldb::DB> db_;
        Schema const & vertexSchema_;

    };

    #define VERTEX_DB_NAME "vertex_data"

    VertexManager::VertexManager(Conf const & conf, Schema const & vertexSchema)
      : reqCount_(0), hitCount_(0),
        vertexDataBufferSize_(conf.vertexDataBufferSize()),
        vertexSchema_(vertexSchema)
    {
        leveldb::Options options;
        options.create_if_missing = true;
        leveldb::DB * db;
        leveldb::Status status = leveldb::DB::Open(options,
                conf.getStorageDir()+"/"+VERTEX_DB_NAME, &db);
        if (!status.ok())
            throw std::runtime_error(status.ToString());
        db_.reset(db);
    }

    #undef VERTEX_DB_NAME

    std::shared_ptr<AttributeData> VertexManager::getVertexData(VertexId id)
    {
        reqCount_++;
        auto it = cache_.find(id);
        if (it!=cache_.end()) {
            hitCount_++;
            VertexDataAndIdIter & dataAndIter = it->second;
            if (dataAndIter.iter != lruList_.begin()) {
                lruList_.erase(dataAndIter.iter);
                lruList_.push_front(id);
                dataAndIter.iter = lruList_.begin();
            }
            return it->second.data;
        } else {
            NetworkByteBuffer keyBuf(sizeof(id));
            keyBuf << id;
            leveldb::Slice key(reinterpret_cast<char *>(keyBuf.getPtr()), keyBuf.getSerializedDataSize());
            std::string value;
            leveldb::Status status = db_->Get(leveldb::ReadOptions(), key, &value);
            if (status.IsNotFound())
                throw vertex_not_found_exception(id);
            else if (!status.ok())
                throw std::runtime_error(status.ToString());
            NetworkByteBuffer dataBuf(reinterpret_cast<unsigned char *>(const_cast<char *>(value.data())), value.size());
            VertexDataAndIdIter & dataAndIter = cache_[id];
            dataAndIter.data.reset(vertexSchema_.newAttributeData());            
            dataBuf >> *dataAndIter.data;
            lruList_.push_front(id);
            dataAndIter.iter = lruList_.begin();
            if (lruList_.size()>=vertexDataBufferSize_) {
                VertexId r = lruList_.back();
                lruList_.pop_back();
                cache_.erase(r); // no need to write back, this is a read only db
            }
            return dataAndIter.data;
        }
    }

    void VertexManager::addVertex(VertexId id, AttributeData * data)
    {
        if (cache_.count(id)!=0)
            throw vertex_already_exists_exception(id);
        NetworkByteBuffer keyBuf(sizeof(id));
        keyBuf << id;
        leveldb::Slice key(reinterpret_cast<char *>(keyBuf.getPtr()), keyBuf.getSerializedDataSize());
        std::string value;
        leveldb::Status status = db_->Get(leveldb::ReadOptions(), key, &value);
        if (status.ok())
            throw vertex_already_exists_exception(id);
        else if (!status.IsNotFound())
            throw std::runtime_error(status.ToString());
        NetworkByteBuffer dataBuf;
        dataBuf << data;
        leveldb::Slice dataSlice(reinterpret_cast<char *>(dataBuf.getPtr()), dataBuf.getSerializedDataSize());
        status = db_->Put(leveldb::WriteOptions(), key, dataSlice);
        if (!status.ok())
            throw std::runtime_error(status.ToString());
    }
} } /* namespace */

#endif /* INTERGDB_VERTEXMANAGER_H */
