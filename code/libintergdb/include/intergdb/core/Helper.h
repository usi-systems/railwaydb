#ifndef INTERGDB_HELPER_H
#define INTERGDB_HELPER_H

#include <intergdb/core/Types.h>
#include <intergdb/core/EdgeData.h>

#include <stdexcept>
#include <typeinfo>
#include <type_traits>
#include <string>

namespace intergdb { namespace core
{
    class Helper
    {
    public:
        static uint64_t getCurrentTimemillis();
        static Timestamp getCurrentTimestamp();
        static Timestamp millisToTimestamp(double millis);
        static std::string exec(std::string cmd);
    };

    template <class T>
    size_t getSerializedSizeOf(T const & val)
    {
        if (std::is_integral<T>::value || std::is_floating_point<T>::value)
            return sizeof(T);
        throw std::runtime_error("Type "+std::string(typeid(T).name())+" does not have getSerializedSizeOf defined for it");
        return 0;
    }

    inline size_t getSerializedSizeOf(std::string const & str)
    {
        return sizeof(uint32_t) + str.size();
    }

    inline size_t getSerializedSizeOf(EdgeData const & data)
    {        
        /*
        size_t size = 0;        
        int i = 0;
    
        Schema & schema = val.getSchema();
        for (auto a : schema.getAttributes()) {
            switch (a.second) {
            case Schema::INT64:
            {
                size += getSerializedSizeOf((int64_t)val.getAttribute(i));
                break;
            }
            case Schema::DOUBLE:
            {
                size += getSerializedSizeOf(std::string("e2-4"));
                break;
            }
            case Schema::STRING:
            {
                size += getSerializedSizeOf(std::string("e2-4"));
                break;
            }
            default:
                assert(false);
            } 
            i++
        }
        return size;
        
        */
        return getSerializedSizeOf(std::string("e2-4"));
    }


} } /* namespace */

#endif /* INTERGDB_HELPER_H */
