#pragma once

#include <intergdb/common/Types.h>

#include <leveldb/db.h>

#include <list>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <limits>

using namespace intergdb::common;

namespace intergdb { namespace core
{

    typedef std::vector<std::unordered_set<std::string>> Partitioning;

    class Block;
    class BlockManager;
    class Conf;
    class IntervalQueryIndex;
    class NetworkByteBuffer;
    class Schema;

    class TimeSlicedPartitioning
    {
    public:
        TimeSlicedPartitioning()
            : TimeSlicedPartitioning(-std::numeric_limits<double>::infinity(),
                                     std::numeric_limits<double>::infinity())
        {}

        // endTime exclusive
        TimeSlicedPartitioning(Timestamp startTime, Timestamp endTime)
            : startTime_(startTime), endTime_(endTime)
        {}

        TimeSlicedPartitioning(Timestamp startTime, Timestamp endTime,
                               Partitioning const & partitioning)
            : startTime_(startTime), endTime_(endTime),
            partitioning_(partitioning)
        {}

        Partitioning const & getPartitioning() const
        {
            return partitioning_;
        }

        Partitioning & getPartitioning()
        {
            return partitioning_;
        }

        Timestamp const & getStartTime() const
        {
            return startTime_;
        }

        Timestamp const & getEndTime() const
        {
            return endTime_;
        }

        Timestamp & getStartTime()
        {
            return startTime_;
        }

        Timestamp & getEndTime()
        {
            return endTime_;
        }

    private:
        Timestamp startTime_;
        Timestamp endTime_;
        Partitioning partitioning_;
    };

    class PartitionIndex
    {
    public:
        PartitionIndex(PartitionIndex const & other) = delete;

        PartitionIndex const & operator=(PartitionIndex const & other) = delete;

        PartitionIndex(Conf const & conf);

        TimeSlicedPartitioning
           getTimeSlicedPartitioningForBlockDeserialization(Timestamp time);

        TimeSlicedPartitioning getTimeSlicedPartitioning(Timestamp time);

        std::vector<TimeSlicedPartitioning> getTimeSlicedPartitionings(
            Timestamp startTime, Timestamp endTime);

        // relacement partitionings must be contigious in time
        void replaceTimeSlicedPartitioning(
            TimeSlicedPartitioning const & toBeReplaced,
            std::vector<TimeSlicedPartitioning> const & replacement);

        // to be replaced partitionings must be contigious in time
        void replaceTimeSlicedPartitionings(
          std::vector<TimeSlicedPartitioning> const & toBeReplaced,
          TimeSlicedPartitioning const & replacement);

        void setIntervalQueryIndex(IntervalQueryIndex * iqIdx)
        {
          iqIdx_ = iqIdx;
        }

        void setBlockManager(BlockManager * bman)
        {
          bman_ = bman;
        }

    private:
        void addPartitioning(TimeSlicedPartitioning const & partitioning);

        void initiatePartitioningRemoval(
            TimeSlicedPartitioning const & partitioning);

        void completePartitioningRemovals();

        void updateBlocks(Timestamp startTime, Timestamp endTime);

        void replaceBlocks(Block const & masterBlock,
                           std::vector<Block> & newBlocks);

        struct PartitioningAndIter
        {
            TimeSlicedPartitioning partitioning;
            std::list<Timestamp>::iterator iter;
        };
        size_t partitioningBufferSize_;
        std::list<Timestamp> lruList_;
        std::map<Timestamp, PartitioningAndIter> cache_;
        Schema const & edgeSchema_;
        IntervalQueryIndex * iqIdx_;
        BlockManager * bman_;
        std::vector<TimeSlicedPartitioning> removedPartitionings_;
        std::auto_ptr<leveldb::DB> db_; // to store the index on disk
    };

    NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf,
                                   TimeSlicedPartitioning const & block);
    NetworkByteBuffer & operator>>(NetworkByteBuffer & sbuf,
                                   TimeSlicedPartitioning & block);
} }



