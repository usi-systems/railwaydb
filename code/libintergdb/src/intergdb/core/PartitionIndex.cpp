#include <intergdb/core/PartitionIndex.h>

#include <intergdb/core/Conf.h>
#include <intergdb/core/Schema.h>
#include <intergdb/core/Exceptions.h>
#include <intergdb/core/NetworkByteBuffer.h>

#include <boost/filesystem.hpp>

using namespace std;
using namespace intergdb::core;

#define PARTITION_DB_NAME "partition_data"

PartitionIndex::PartitionIndex(Conf const & conf)
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
      Schema const & schema = conf.getEdgeSchema();
      unordered_set<string> allAttributes;
      auto const & attributes = schema.getAttributes();
      for (auto const & attribute : attributes)
        allAttributes.insert(attribute.first);
      TimeSlicedPartitioning initialPartitioning;
      initialPartitioning.getPartitioning().push_back(std::move(allAttributes));
      addPartitioning(initialPartitioning);
    }
}

void PartitionIndex::addPartitioning(TimeSlicedPartitioning const & partitioning)
{
  

}

void replaceTimeSlicedPartitioning(TimeSlicedPartitioning const & toReplace, 
  std::vector<TimeSlicedPartitioning> const & replacement)
{

}

// to be replaced partitionings must be contigious in time
void replacePartitionings(std::vector<TimeSlicedPartitioning> const & toReplace, 
  Partitioning const & replacement)
{

}

