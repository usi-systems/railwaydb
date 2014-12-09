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

  class Conf;
  class NetworkByteBuffer;

  typedef std::vector<std::unordered_set<std::string>> Partitioning;
  
  class TimeSlicedPartitioning
  {
  public:
    TimeSlicedPartitioning() : TimeSlicedPartitioning(
      -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()) {};
    TimeSlicedPartitioning(Timestamp startTime, Timestamp endTime) // endTime exclusive
      : startTime_(startTime), endTime_(endTime) {}
    TimeSlicedPartitioning(Timestamp startTime, Timestamp endTime, Partitioning const & partitioning)
      : startTime_(startTime), endTime_(endTime), partitioning_(partitioning) {}    
    Partitioning const & getPartitioning() const { return partitioning_; }
    Partitioning & getPartitioning() { return partitioning_; }
    Timestamp const & getStartTime() const { return startTime_; }
    Timestamp const & getEndTime() const { return endTime_; }
    Timestamp & getStartTime() { return startTime_; }
    Timestamp & getEndTime() { return endTime_; }
  private:      
    Timestamp startTime_;
    Timestamp endTime_;
    Partitioning partitioning_;
  };

  class PartitionIndex
  {
  public:
    PartitionIndex(Conf const & conf);
    TimeSlicedPartitioning getTimeSlicedPartitioning(Timestamp time);
    std::vector<TimeSlicedPartitioning> getTimeSlicedPartitionings(Timestamp startTime, Timestamp endTime);
    // relacement partitionings must be contigious in time
    void replaceTimeSlicedPartitioning(TimeSlicedPartitioning const & toBeReplaced, 
        std::vector<TimeSlicedPartitioning> const & replacement);
    // to be replaced partitionings must be contigious in time
    void replacePartitionings(std::vector<TimeSlicedPartitioning> const & toBeReplaced, 
        TimeSlicedPartitioning const & replacement);
  private:
    void addPartitioning(TimeSlicedPartitioning const & partitioning);
    void removePartitioning(TimeSlicedPartitioning const & partitioning);
  private:
    struct PartitioningAndIter 
    {
      TimeSlicedPartitioning partitioning;
      std::list<Timestamp>::iterator iter;
    }; 
    size_t partitioningBufferSize_;
    std::list<Timestamp> lruList_;
    std::map<Timestamp, PartitioningAndIter> cache_;
    std::auto_ptr<leveldb::DB> db_; // to store the index on disk
  };

  NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf, TimeSlicedPartitioning const & block);
  NetworkByteBuffer & operator>>(NetworkByteBuffer & sbuf, TimeSlicedPartitioning & block);

} }



