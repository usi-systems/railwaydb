#include <intergdb/common/Partitioning.h>

#include <string>
//#include <unordered_map>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;

string Partition::toString() const
{ 
  if (attributes_.size()==0)
    return "[attributes: ]";
  auto it = attributes_.begin();
  string str = "[attributes: " + (*(it++))->toString();
  for (; it!=attributes_.end(); ++it) 
    str += "," + (*it)->toString();
  str += "]";
  return str;
}

void Partitioning::mergePartitions(int i, int j)
{
  if (j<i) {
    mergePartitions(j, i);
  } else {
    for (Attribute const * attrb : partitions_[j].getAttributes())
      partitions_[i].addAttribute(attrb);
    if (j!=partitions_.size()-1) 
      std::swap(partitions_[j], *partitions_.rbegin());
    partitions_.pop_back();
  }
}

string Partitioning::toString() const
{ 
  if (partitions_.size()==0)
    return "{partitions: }";
  auto it = partitions_.begin();
  string str = "{partitions: " + (it++)->toString();
  for (; it!=partitions_.end(); ++it) 
    str += "," + it->toString(); 
  str += "}";
  return str;
}


