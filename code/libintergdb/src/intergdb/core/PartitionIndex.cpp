#include <intergdb/core/PartitionIndex.h>

#include <intergdb/core/Conf.h>
#include <intergdb/core/Schema.h>
#include <intergdb/core/Exceptions.h>
#include <intergdb/core/NetworkByteBuffer.h>

#include <boost/filesystem.hpp>
#include <memory>

using namespace std;
using namespace intergdb::core;

#define PARTITION_DB_NAME "partition_data"

PartitionIndex::PartitionIndex(Conf const & conf)
  : partitioningBufferSize_(conf.partitioningBufferSize())
{
    leveldb::Options options;
    options.create_if_missing = true;
    options.max_open_files = 100;
    leveldb::DB * db;
    string dbPath = conf.getStorageDir()+ "/" + PARTITION_DB_NAME;
    bool newDB = !boost::filesystem::exists(dbPath);
    leveldb::Status status = leveldb::DB::Open(options, dbPath, &db);
    if (!status.ok())
        throw std::runtime_error(status.ToString());
    db_.reset(db);
    if (newDB) {
      TimeSlicedPartitioning initialPartitioning{};
      initialPartitioning.getPartitioning() = conf.getPartitioning();
      addPartitioning(initialPartitioning);
    }
}

void PartitionIndex::addPartitioning(TimeSlicedPartitioning const & partitioning)
{
  NetworkByteBuffer keyBuf(sizeof(Timestamp));
  keyBuf << partitioning.getEndTime();
  leveldb::Slice key(reinterpret_cast<char *>(keyBuf.getPtr()), keyBuf.getSerializedDataSize());
  NetworkByteBuffer dataBuf;
  dataBuf << partitioning;
  leveldb::Slice dataSlice(reinterpret_cast<char *>(dataBuf.getPtr()), dataBuf.getSerializedDataSize());
  leveldb::Status status = db_->Put(leveldb::WriteOptions(), key, dataSlice);
  if (!status.ok())
    throw std::runtime_error(status.ToString());
} 

void  PartitionIndex::removePartitioning(TimeSlicedPartitioning const & partitioning)
{
  NetworkByteBuffer keyBuf(sizeof(Timestamp));
  keyBuf << partitioning.getEndTime();
  leveldb::Slice key(reinterpret_cast<char *>(keyBuf.getPtr()), keyBuf.getSerializedDataSize());
  leveldb::Status status = db_->Delete(leveldb::WriteOptions(), key);
  if (!status.ok())
    throw std::runtime_error(status.ToString());
  auto it = cache_.find(partitioning.getEndTime());
  if (it!=cache_.end()) {  
    PartitioningAndIter & partingAndIter = it->second;
    lruList_.erase(partingAndIter.iter);
    cache_.erase(it);
  }
}

TimeSlicedPartitioning PartitionIndex::getTimeSlicedPartitioning(Timestamp time) 
{
  auto cit = cache_.lower_bound(time);
  if (cit!=cache_.end()) {    
    PartitioningAndIter & partingAndIter = cit->second;
    auto const & partitioning = partingAndIter.partitioning;
    if (time>=partitioning.getStartTime() && time<partitioning.getEndTime()) {
      if (partingAndIter.iter != lruList_.begin()) {
        lruList_.erase(partingAndIter.iter);
        lruList_.push_front(time);
        partingAndIter.iter = lruList_.begin();
      }
      return partitioning;
    }
  }
  // not in the cache, so go to the DB
  NetworkByteBuffer searchKeyBuf(sizeof(Timestamp));
  searchKeyBuf << time;
  leveldb::Slice searchKey(reinterpret_cast<char *>(searchKeyBuf.getPtr()), searchKeyBuf.getSerializedDataSize());
  unique_ptr<leveldb::Iterator> it(db_->NewIterator(leveldb::ReadOptions()));
  it->Seek(searchKey); 
  if (!it->Valid()) 
    throw time_sliced_partition_not_found_exception(time);
  leveldb::Slice foundKey = it->key();
  NetworkByteBuffer foundKeyBuf(reinterpret_cast<unsigned char *>(const_cast<char *>(foundKey.data())), foundKey.size());
  Timestamp endTime;
  foundKeyBuf >> endTime;   
  if (endTime == time) {
    it->Next();
    if (!it->Valid()) 
      throw time_sliced_partition_not_found_exception(time);
  }
  leveldb::Slice value = it->value();
  NetworkByteBuffer valueBuf(reinterpret_cast<unsigned char *>(const_cast<char *>(value.data())), value.size());
  PartitioningAndIter & partingAndIter = cache_.emplace(endTime, PartitioningAndIter()).first->second;
  valueBuf >> partingAndIter.partitioning;
  lruList_.push_front(endTime);
  partingAndIter.iter = lruList_.begin();
  if (lruList_.size()>partitioningBufferSize_) {
      Timestamp t = lruList_.back();
      lruList_.pop_back();
      cache_.erase(t); 
  }
  return partingAndIter.partitioning;
}

vector<TimeSlicedPartitioning> PartitionIndex::getTimeSlicedPartitionings(Timestamp startTime, Timestamp endTime) 
{
  vector<TimeSlicedPartitioning> results;
  Timestamp time = startTime;
  do {
    TimeSlicedPartitioning partitioning = getTimeSlicedPartitioning(time);
    time = partitioning.getEndTime();
    results.push_back(std::move(partitioning));  
  } while (time < endTime);
  return results;
}

// relacement partitionings must be contigious in time
void PartitionIndex::replaceTimeSlicedPartitioning(TimeSlicedPartitioning const & toBeReplaced, 
    std::vector<TimeSlicedPartitioning> const & replacement)
{
  if (replacement.empty())
    throw time_sliced_partition_replacement_exception("replacement time slice sequence is empty");
  if (toBeReplaced.getStartTime() != replacement.begin()->getStartTime())
    throw time_sliced_partition_replacement_exception("start time of the time slice to be replaced does not match that of the replacement time slice sequence");
  if (toBeReplaced.getEndTime() != replacement.rbegin()->getEndTime())
    throw time_sliced_partition_replacement_exception("end time of the time slice to be replaced does not match that of the replacement time slice sequence");
  for (size_t i=1, iu=replacement.size(); i<iu; ++i) 
    if (replacement[i].getStartTime()!=replacement[i-1].getEndTime())
      throw time_sliced_partition_replacement_exception("replacement time slice sequence is not contigious in time");
  // all is ok, proceed to replace
  removePartitioning(toBeReplaced);
  for (auto const & partitioning : replacement)
    addPartitioning(partitioning);
}

// to be replaced partitionings must be contigious in time
void PartitionIndex::replacePartitionings(std::vector<TimeSlicedPartitioning> const & toBeReplaced, 
    TimeSlicedPartitioning const & replacement)
{
  if (toBeReplaced.empty())
    throw time_sliced_partition_replacement_exception("time slice sequence to be replaced is empty");
  if (toBeReplaced.begin()->getStartTime() != replacement.getStartTime())
    throw time_sliced_partition_replacement_exception("start time of the time slice sequence to be replaced does not match that of the replacement time slice");
  if (toBeReplaced.rbegin()->getEndTime() != replacement.getEndTime())
    throw time_sliced_partition_replacement_exception("end time of the time slice sequence to be replaced does not match that of the replacement time slice");
  for (size_t i=1, iu=toBeReplaced.size(); i<iu; ++i) 
    if (toBeReplaced[i].getStartTime()!=toBeReplaced[i-1].getEndTime())
      throw time_sliced_partition_replacement_exception("replacement time slice sequence is not contigious in time");
  // all is ok, proceed to replace
  for (auto const & partitioning : toBeReplaced)
    removePartitioning(partitioning);
  addPartitioning(replacement);
}

namespace intergdb { namespace core {

NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf, TimeSlicedPartitioning const & tparting)
{
  sbuf << tparting.getStartTime();
  sbuf << tparting.getEndTime();
  auto const & partitioning = tparting.getPartitioning();
  uint32_t nParts = partitioning.size();
  sbuf << nParts;
  for (auto const & attributes : partitioning) {
    uint32_t nAttributes = attributes.size();
    sbuf << nAttributes;
    for (string const & attribute : attributes)
      sbuf << attribute;
  }
  return sbuf;
}

NetworkByteBuffer & operator>>(NetworkByteBuffer & sbuf, TimeSlicedPartitioning & tparting)
{
  sbuf >> tparting.getStartTime();
  sbuf >> tparting.getEndTime();
  auto & partitioning = tparting.getPartitioning();
  uint32_t nParts;
  sbuf >> nParts;
  for (size_t i=0; i<nParts; ++i) {
    uint32_t nAttributes;
    sbuf >> nAttributes;
    unordered_set<string> attributes;
    for (size_t j=0; j<nAttributes; ++j) {
      string attribute;
      sbuf >> attribute;
      attributes.insert(std::move(attribute));
    }
    partitioning.push_back(std::move(attributes));
  }
  return sbuf;
}

} }





