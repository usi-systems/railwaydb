#ifndef INTERGDB_EDGEDATA_H
#define INTERGDB_EDGEDATA_H

namespace intergdb { namespace core
{
    class EdgeData
    {
    public:
        EdgeData() { }
        void addAttribute(int64_t value) const { }
        void addAttribute(double value) const { }
        void addAttribute(String value) const { }
    };
} } /* namespace */

#endif /* INTERGDB_EDGE_H */
