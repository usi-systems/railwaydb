#include <intergdb/common/SchemaStats.h>

#include <iostream>
using namespace std;

std::ostream & operator<<(std::ostream & out, intergdb::common::SchemaStats const & stats)
{
    int size = stats.getStats().size();
    out << size;
    for (auto indexToCountAndBytesPair : stats.getStats()) {
        out << indexToCountAndBytesPair.first;
        out << indexToCountAndBytesPair.second.first;
        out << indexToCountAndBytesPair.second.second;
    }
    return out;
}

std::istream& operator>>(std::istream& is, intergdb::common::SchemaStats & stats)
{
    int numStats;
    int index;
    int count;
    double bytes;
    std::unordered_map<int, std::pair<int,double> > indexToCountAndBytes;
    is >> numStats;
    while (numStats) {
        is >> index;
        is >> count;
        is >> bytes;
        numStats--;
        indexToCountAndBytes.emplace(index, std::make_pair(count, bytes));
    }
    stats.setStats(indexToCountAndBytes);
    return is;
}
