#pragma once

#include <intergdb/common/Types.h>

#include <spatialindex/SpatialIndex.h>
#include <spatialindex/RTree.h>

#include <boost/bind.hpp>
#include <boost/coroutine/coroutine.hpp>

using namespace intergdb::common;

namespace intergdb { namespace core
{
    class RTreeIntervalIndex
    {
    public:
        class IntervalData
        {
        public:
            static std::unique_ptr<SpatialIndex::Region> getRegion(
                    Timestamp const start, Timestamp const end,
                    Timestamp yStart, Timestamp yEnd);
        public:
            IntervalData()
            {}

            IntervalData(BlockId id, VertexId vertex,
                         Timestamp start, Timestamp end)
                : id_(id), vertex_(vertex), start_(start), end_(end)
            {}

            std::unique_ptr<SpatialIndex::Region> getRegion() const
            {
                return IntervalData::getRegion(start_, end_, 1.0, 9.0);
            }

            std::unique_ptr<unsigned char[]> getBytes(uint32_t & size) const;

            void load(SpatialIndex::IData const & data);

            BlockId const & getBlockId() const
            {
                return id_;
            }

            VertexId const & getVertex() const
            {
                return vertex_;
            }

            Timestamp const & getIntervalStart() const
            {
                return start_;
            }

            Timestamp const & getIntervalEnd() const
            {
                return end_;
            }

        private:
            BlockId id_;
            VertexId vertex_;
            Timestamp start_;
            Timestamp end_;
        };
    public:
        class Iterator
        {
        private:
            typedef boost::coroutines::symmetric_coroutine<void> boost_coro;
            typedef boost_coro::call_type coro_t;
            typedef boost_coro::yield_type caller_t;
        public:
            Iterator(SpatialIndex::ISpatialIndex & rtidx,
                     Timestamp from, Timestamp to)
                : valid_(true), from_(from), to_(to), rtidx_(rtidx),
                  coro_(boost::bind(&Iterator::visitCoro, this, _1))
            {
                coro_();
            }

            bool isValid()
            {
                return valid_;
            }

            void moveToNext()
            {
                if(valid_)
                    coro_();
            }

            IntervalData const & getData()
            {
                return visitor_.getData();
            }

        private:
            class Visitor : public SpatialIndex::IVisitor
            {
            public:
                void visitNode(SpatialIndex::INode const & node)
                {}

                void visitData(std::vector<const SpatialIndex::IData*>& v)
                {}

                void visitData(SpatialIndex::IData const & data)
                {
                    data_.load(data);
                    (*caller_)(); // return back to caller
                }

                void setCaller(caller_t & caller)
                {
                    caller_ = &caller;
                }

                IntervalData const & getData()
                {
                    return data_;
                }

            private:
                IntervalData data_;
                caller_t * caller_;
            };

            void visitCoro(caller_t & caller)
            {
                visitor_.setCaller(caller);
                std::unique_ptr<SpatialIndex::Region> region =  IntervalData::getRegion(from_, to_, 0.0, 10.0);
                rtidx_.intersectsWithQuery(*region, visitor_);
                valid_ = false;
            }

        private:
            bool valid_;
            Visitor visitor_;
            Timestamp from_, to_;
            SpatialIndex::ISpatialIndex & rtidx_;
            coro_t coro_;
        };

    public:
        RTreeIntervalIndex()
            : nextId_(0)
        {}

        ~RTreeIntervalIndex();

        void openOrCreate(std::string const & baseName);

        Iterator * getNewIterator(
            Timestamp const & start, Timestamp const & end)
        {
            return new Iterator(*rtidx_, start, end);
        }

        void addInterval(BlockId id, VertexId vertex,
                         Timestamp const & start, Timestamp const & end);

        void queryBatch(Timestamp start, Timestamp end,
                        std::vector<VertexId> & results);

    private:
        size_t nextId_;
        SpatialIndex::StorageManager::IBuffer * buffer_;
        SpatialIndex::IStorageManager* disk_;
        std::unique_ptr<SpatialIndex::ISpatialIndex> rtidx_;
    };

} } /* end namespace */


