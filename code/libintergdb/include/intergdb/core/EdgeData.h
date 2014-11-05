#ifndef INTERGDB_EDGEDATA_H
#define INTERGDB_EDGEDATA_H

#include <string>

namespace intergdb { namespace core
{
    class EdgeData
    {
    public:
        EdgeData() { }
        EdgeData(int value) { }
        EdgeData(int64_t value) { }
        EdgeData(double value) { }
        EdgeData(std::string value) { }
        void addAttribute(int64_t value) const { }
        void addAttribute(double value) const { }
        void addAttribute(std::string value) const { }
        std::string toString() const { return "EdgeData"; }
        bool operator=(const EdgeData*) { return true; }
    };
} } /* namespace */

#endif /* INTERGDB_EDGE_H */
