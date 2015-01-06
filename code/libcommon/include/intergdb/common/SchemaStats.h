#pragma once

#include <unordered_map>

namespace intergdb { namespace common
{     
    class SchemaStats
    {        
    public:
        SchemaStats() {}

        double getAvgSize(int index) 
        {
            auto it = indexToCountAndBytes_.find(index);
            if ( it == indexToCountAndBytes_.end() ) {
                throw std::runtime_error("SchemaStats index not found.");
            }
            if (it->second.first == 0) return 0.0;
            else return (it->second.second / it->second.first);
        }

        void incrCountAndBytes(int index, double bytes) 
        {
            auto it = indexToCountAndBytes_.find(index);
            if ( it == indexToCountAndBytes_.end() ) {
                indexToCountAndBytes_.emplace(index, std::make_pair(1, bytes));
            } else {
                it->second.first++;
                it->second.second+=bytes;
            }            
        }

    private:
        std::unordered_map<int, std::pair<int,double> > indexToCountAndBytes_; 
    };
} } /* namespace */

