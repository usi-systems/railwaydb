#pragma once

#include <intergdb/common/Partition.h>

#include <vector>

namespace intergdb { namespace common
{
    class Partitioning
    {
    public:
        Partitioning()
        {}

        void addPartition(Partition const & partition)
        {
            partitions_.push_back(partition);
        }

        int numPartitions()
        {
            return partitions_.size();
        }

        std::vector<Partition> const & getPartitions() const
        {
            return partitions_;
        }

        bool operator==(Partitioning const & other) const;

        void mergePartitions(int i, int j);

        void splitPartition(int i, int j,
                            Partition const & lhs, Partition const & rhs);

        std::string toString() const;

        std::vector<std::unordered_set<std::string>> toStringSet() const;

    private:
        std::vector<Partition> partitions_;
    };
} }
