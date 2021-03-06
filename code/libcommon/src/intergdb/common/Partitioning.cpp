#include <intergdb/common/Partitioning.h>

#include <string>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;

bool Partition::operator==(Partition const & other) const
{
    auto const & oattrbs = other.getAttributes();
    if (oattrbs.size()!=attributes_.size())
        return false;
    for (auto const attrb : attributes_)
        if (oattrbs.count(attrb)==0)
            return false;
    return true;
}

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

std::vector<std::unordered_set<std::string>> Partitioning::toStringSet() const
{
    std::vector<std::unordered_set<std::string>> part;

    for (Partition p : partitions_) {
        std::unordered_set<std::string> attrSet;
        for (Attribute const * a : p.getAttributes())
            attrSet.emplace(a->getName());
        part.push_back(attrSet);
    }
    return part;

}

void Partitioning::mergePartitions(int i, int j)
{
    if (j<i)
        return mergePartitions(j, i);
    for (Attribute const * attrb : partitions_[j].getAttributes())
        partitions_[i].addAttribute(attrb);
    if (j!=partitions_.size()-1)
        std::swap(partitions_[j], *partitions_.rbegin());
    partitions_.pop_back();
}

void Partitioning::splitPartition(int i, int j,
  Partition const & lhs, Partition const & rhs)
{
    if (j<i)
        return splitPartition(j, i, rhs, lhs);
    Partition & part = partitions_[i];
    part.clearAttributes();
    for (Attribute const * attrb : lhs.getAttributes())
        part.addAttribute(attrb);
    addPartition(rhs);
    if (j!=partitions_.size()-1)
        std::swap(partitions_[j], *partitions_.rbegin());
}

bool Partitioning::operator==(Partitioning const & other) const
{
    vector<Partition> const & oparts = other.getPartitions();
    if (oparts.size()!=partitions_.size())
        return false;
    for (size_t i=0, iu=partitions_.size(); i<iu; ++i)
        if (!(partitions_[i]==oparts[i]))
          return false;
    return true;
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
