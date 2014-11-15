#pragma once

#include <intergdb/core/Types.h>
#include <intergdb/core/Edge.h>

#include <deque>

namespace intergdb { namespace core
{
    class EdgeFIFO
    {
    public:
        EdgeFIFO(size_t windowSize) : windowSize_(windowSize) {}
        void addEdge(UEdge const & edge) { edges_.push_back(edge); }
        bool isEmpty() const { return edges_.empty(); }
        bool hasExpiredEdges() const { return (edges_.size()>windowSize_); }
        UEdge & getOldestEdge() { return edges_.front(); }
        void popOldestEdge() { edges_.erase(edges_.begin()); }
    private:
        size_t windowSize_;
        std::deque<UEdge> edges_;
    };
} } /* namespace */
