#ifndef INTERGDB_ENDIANHELPER_H
#define INTERGDB_ENDIANHELPER_H

#ifdef __MACH__
#include <machine/endian.h>
#elif defined __linux__
#include <endian.h>
#endif
#include <inttypes.h>
#include <netinet/in.h>

namespace intergdb { namespace core {

    template <typename T>
    union HostNetworkTmp
    {
        T data;
        char rawData[sizeof (T)];
    }
    ;
    template <typename T>
    inline HostNetworkTmp<T> host_to_network(const T& v)
    {
        HostNetworkTmp<T> dst;
    #if BYTE_ORDER != LITTLE_ENDIAN
        dst.data = v;
        return dst;
    #else
        const HostNetworkTmp<T> & src = reinterpret_cast<const HostNetworkTmp<T>&>(v);
        for (uint32_t i=0; i<sizeof (src); ++i)
            dst.rawData[i] = src.rawData[sizeof (src)-i-1];
        return dst;
    #endif
    }

    template <typename T>
    inline HostNetworkTmp<T> network_to_host(const T& v)
    {
        return host_to_network(v);
    }

    #if BYTE_ORDER == LITTLE_ENDIAN
    // Specializations for performance
    template<>
    inline HostNetworkTmp<int16_t> host_to_network(const int16_t& v)
    {
        HostNetworkTmp<int16_t> dst;
        dst.data = htons(v);
        return dst;
    }
    template<>
    inline HostNetworkTmp<uint16_t> host_to_network(const uint16_t& v)
    {
        HostNetworkTmp<uint16_t> dst;
        dst.data = htons(v);
        return dst;
    }
    template<>
    inline HostNetworkTmp<int32_t> host_to_network(const int32_t& v)
    {
        HostNetworkTmp<int32_t> dst;
        dst.data = htonl(v);
        return dst;
    }
    template<>
    inline HostNetworkTmp<uint32_t> host_to_network(const uint32_t& v)
    {
        HostNetworkTmp<uint32_t> dst;
        dst.data = htonl(v);
        return dst;
    }
    #endif
} } /* namespace */

#endif /* INTERGDB_ENDIANHELPER_H */
