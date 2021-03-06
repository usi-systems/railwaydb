#pragma once

#include <unordered_map>

#include <string>
#include <sstream>

namespace intergdb { namespace common
{
    class SchemaStats
    {
    public:
        SchemaStats()
        {}

        double getAvgSize(uint32_t index) const
        {
            auto it = indexToCountAndBytes_.find(index);
            if (it == indexToCountAndBytes_.end())
                throw std::runtime_error("SchemaStats index " +
                    std::to_string(index) + " not found.");
            if (it->second.first == 0)
                return 0.0;
            else
                return (it->second.second / it->second.first);
        }

        void incrCountAndBytes(uint32_t index, double bytes)
        {
            auto it = indexToCountAndBytes_.find(index);
            if (it == indexToCountAndBytes_.end()) {
                indexToCountAndBytes_[index] = std::make_pair(1, bytes);
            } else {
                it->second.first++;
                it->second.second += bytes;
            }
        }

        std::unordered_map<uint32_t, std::pair<uint32_t,double> > const & getStats() const
        {
            return indexToCountAndBytes_;
        }

        std::unordered_map<uint32_t, std::pair<uint32_t,double> > & getStats()
        {
            return indexToCountAndBytes_;
        }

        std::string toString() {
            std::stringstream ss;
            ss << "SchemaStats:" << std::endl;
            for (auto it : indexToCountAndBytes_) {
                ss << "\t" << it.first << "\t" << getAvgSize(it.first) << std::endl;
            }
            return ss.str();
        }

    private:
        std::unordered_map<uint32_t, std::pair<uint32_t,double> > indexToCountAndBytes_;
    };
} } /* namespace */


