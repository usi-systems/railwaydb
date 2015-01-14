#include <intergdb/core/MetaDataManager.h>
#include <intergdb/core/NetworkByteBuffer.h>
#include <intergdb/common/SchemaStats.h>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace std;
using namespace intergdb::common;
using namespace intergdb::core;

#define STATS_DATA "schemastats_data"

MetaDataManager::MetaDataManager(std::string const & storageDir) 
  : storageDir_(storageDir), nextBlockId_(0) 
{
    load();
}

void MetaDataManager::store() 
{
    NetworkByteBuffer buf;
    buf << nextBlockId_;
    buf << schemaStats_;
    std::ofstream file(storageDir_+"/"+STATS_DATA, ios::out|std::ios::binary);
    if ( !file.is_open() ) 
        throw runtime_error("Could not open file: " + storageDir_+"/"+STATS_DATA);   
    file.write(reinterpret_cast<char *>(buf.getPtr()), buf.getSerializedDataSize());
    file.close();
}

void MetaDataManager::load() 
{            
    if (!exists(storageDir_+"/"+STATS_DATA)) 
        return;
    std::ifstream file(storageDir_+"/"+STATS_DATA, ios::in|ios::binary|ios::ate);
    if (!file.is_open()) 
        throw runtime_error("Could not open file: " + storageDir_+"/"+STATS_DATA); 
    streampos size = file.tellg();
    NetworkByteBuffer buf(size);
    file.seekg (0, ios::beg);
    file.read (reinterpret_cast<char *>(buf.getPtr()), size);
    file.close();
    buf >> nextBlockId_;
    buf >> schemaStats_;
}

namespace intergdb { namespace core
{

NetworkByteBuffer & operator>>(NetworkByteBuffer & sbuf, SchemaStats & stats)
{
    uint32_t numStats = 0;
    uint32_t index;
    uint32_t count;
    double bytes;
    sbuf >> numStats;
    while (numStats) {
        sbuf >> index;
        sbuf >> count;
        sbuf >> bytes;      
        numStats--;
        stats.getStats().emplace(index, std::make_pair(count, bytes));
    }
    return sbuf;
}

NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf, SchemaStats const & stats)
{

    uint32_t size = stats.getStats().size();
    sbuf << size;
    for (auto indexToCountAndBytesPair : stats.getStats()) {
        sbuf << indexToCountAndBytesPair.first;
        sbuf << indexToCountAndBytesPair.second.first;
        sbuf << indexToCountAndBytesPair.second.second;
    }
    return sbuf;
}

} }
