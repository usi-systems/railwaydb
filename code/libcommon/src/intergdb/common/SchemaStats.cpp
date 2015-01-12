#include <intergdb/common/SchemaStats.h>
#include <intergdb/core/NetworkByteBuffer.h>

#include <iostream>
using namespace std;

NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf, intergdb::common::SchemaStats const & stats)
{
    int size = stats.getStats().size();
    sbuf << size;
    for (auto indexToCountAndBytesPair : stats.getStats()) {
        sbuf << indexToCountAndBytesPair.first;
        sbuf << indexToCountAndBytesPair.second.first;
        sbuf << indexToCountAndBytesPair.second.second;
    }
    return out;
}

NetworkByteBuffer & operator>>(NetworkByteBuffer & sbuf, intergdb::common::SchemaStats & stats)
{
    int numStats;
    int index;
    int count;
    double bytes;
    std::unordered_map<int, std::pair<int,double> > indexToCountAndBytes;
    sbuf >> numStats;
    while (numStats) {
        sbuf >> index;
        sbuf >> count;
        sbuf >> bytes;
        numStats--;
        indexToCountAndBytes.emplace(index, std::make_pair(count, bytes));
    }
    stats.setStats(indexToCountAndBytes);
    return sbuf;
}
