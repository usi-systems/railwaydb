#pragma once

#include <intergdb/common/Types.h>

#include <leveldb/db.h>

#include <memory>

using namespace intergdb::common;

namespace intergdb { namespace core
{
    class Conf;
    class Block;

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
        void indexBlock(Block const & block);
        std::shared_ptr<Iterator> query(VertexId vertex, Timestamp start, Timestamp end);
    private:
        std::auto_ptr<leveldb::DB> db_;
    };

} } /* namespace */


