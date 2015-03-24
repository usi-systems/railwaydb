#pragma once

#include <intergdb/core/AttributeData.h>
#include <intergdb/core/EndianHelper.h>
#include <intergdb/core/Schema.h>

#include <algorithm>
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

namespace intergdb { namespace core
{
    class NetworkByteBuffer
    {
    public:
        enum { DEFAULT_SIZE = 4096 };

        NetworkByteBuffer();

        NetworkByteBuffer(uint64_t initialSize);

        NetworkByteBuffer(unsigned char * mybuf, uint64_t mysize);

        NetworkByteBuffer(NetworkByteBuffer const & sb);

        NetworkByteBuffer(NetworkByteBuffer & sb, bool isDestructive);

        ~NetworkByteBuffer();

        NetworkByteBuffer & assign(NetworkByteBuffer const & sb);

        NetworkByteBuffer & copyData(NetworkByteBuffer const & sb);

        void addChar(char ch)
        {
            ensure(sizeof(char));
            buf[icursor] = ch;
            icursor++;
        }

        char getChar()
        {
            char ch = buf[ocursor];
            ocursor++;
            return ch;
        }

        void addBool(bool b)
        {
            ensure(sizeof(bool));
            buf[icursor] = b;
            icursor++;
        }

        bool getBool()
        {
            bool b = buf[ocursor];
            ocursor++;
            return b;
        }

        void addUInt8(uint8_t i)
        {
            ensure(sizeof(uint8_t));
            buf[icursor] = i;
            icursor++;
        }

        uint8_t getUInt8()
        {
            char i = buf[ocursor];
            ocursor++;
            return i;
        }

        void addInt8(int8_t i)
        {
            ensure(sizeof(int8_t));
            buf[icursor] = i;
            icursor++;
        }

        int8_t getInt8()
        {
            char i = buf[ocursor];
            ocursor++;
            return i;
        }

        void addUInt16(uint16_t i)
        {
            ensure(sizeof(uint16_t));
            memcpy(buf+icursor, host_to_network<uint16_t>(i).rawData,
                   sizeof(uint16_t));
            icursor += sizeof(uint16_t);
        }

        uint16_t getUInt16()
        {
            uint16_t temp = network_to_host<uint16_t>(
                *reinterpret_cast<uint16_t *>(buf+ocursor)).data;
            ocursor += sizeof(uint16_t);
            return temp;
        }

        void addInt16(int16_t i)
        {
            ensure(sizeof(int16_t));
            memcpy(buf+icursor, host_to_network<int16_t>(i).rawData,
                   sizeof(int16_t));
            icursor += sizeof(int16_t);
        }

        int16_t getInt16()
        {
            int16_t temp = network_to_host<int16_t>(
                *reinterpret_cast<int16_t *>(buf+ocursor)).data;
            ocursor += sizeof(int16_t);
            return temp;
        }

        void addUInt32(uint32_t i)
        {
            ensure(sizeof(uint32_t));
            memcpy(buf+icursor, host_to_network<uint32_t>(i).rawData,
                   sizeof(uint32_t));
            icursor += sizeof(uint32_t);
        }

        uint32_t getUInt32()
        {
            uint32_t temp = network_to_host<uint32_t>(
                *reinterpret_cast<uint32_t *>(buf+ocursor)).data;
            ocursor += sizeof(uint32_t);
            return temp;
        }

        void addInt32(int32_t i)
        {
            ensure(sizeof(int32_t));
            memcpy(buf+icursor, host_to_network<int32_t>(i).rawData,
                   sizeof(int32_t));
            icursor += sizeof(int32_t);
        }

        int32_t getInt32()
        {
            int32_t temp = network_to_host<int32_t>(
                *reinterpret_cast<int32_t *>(buf+ocursor)).data;
            ocursor += sizeof(int32_t);
            return temp;
        }

        void addUInt64(uint64_t i)
        {
            ensure(sizeof(uint64_t));
            memcpy(buf+icursor, host_to_network<uint64_t>(i).rawData,
                   sizeof(uint64_t));
            icursor += sizeof(uint64_t);
        }

        uint64_t getUInt64()
        {
            uint64_t temp = network_to_host<uint64_t>(
                *reinterpret_cast<uint64_t *>(buf+ocursor)).data;
            ocursor += sizeof(uint64_t);
            return temp;
        }

        void addInt64(int64_t i)
        {
            ensure(sizeof(int64_t));
            memcpy(buf+icursor, host_to_network<int64_t>(i).rawData,
                   sizeof(int64_t));
            icursor += sizeof(int64_t);
        }

        int64_t getInt64()
        {
            int64_t temp = network_to_host<int64_t>(
                *reinterpret_cast<int64_t *>(buf+ocursor)).data;
            ocursor += sizeof(int64_t);
            return temp;
        }

        void addFloat(float f)
        {
            ensure(sizeof(float));
            memcpy(buf+icursor, host_to_network<float>(f).rawData,
                   sizeof(float));
            icursor += sizeof(float);
        }

        float getFloat()
        {
            float temp = network_to_host<float>(
                *reinterpret_cast<float *>(buf+ocursor)).data;
            ocursor += sizeof(float);
            return temp;
        }

        void addDouble(double d)
        {
            ensure(sizeof(double));
            memcpy(buf+icursor,
                   host_to_network<double>(d).rawData,sizeof(double));
            icursor += sizeof(double);
        }

        double getDouble()
        {
            double temp = network_to_host<double>(
                *reinterpret_cast<double *>(buf+ocursor)).data;
            ocursor += sizeof(double);
            return temp;
        }

        void addNTStr(char const * str)
        {
            // adds a NULL Terminated String
            uint32_t stlen = strlen(str)+1;
            ensure(stlen);
            memcpy(buf+icursor,str,stlen);
            icursor += stlen;
        }

        char * getNTStr()
        {
            uint32_t stlen = strlen((char *)buf+ocursor)+1;
            char * ptr = reinterpret_cast<char *>(buf)+ocursor;
            ocursor += stlen;
            return ptr;
        }

        void addSTLString(std::string const & str)
        {
            uint32_t stlen = str.size();
            addUInt32(stlen);
            if(stlen>0) {
                ensure(stlen);
                memcpy(buf+icursor,str.c_str(),stlen);
                icursor += stlen;
            }
        }

        std::string getSTLString()
        {
            uint32_t stlen = getUInt32();
            if(stlen>0) {
                char * ptr = reinterpret_cast<char *>(buf)+ocursor;
                ocursor += stlen;
                return std::string(ptr,stlen);
            }
            return std::string("");
        }

        void addBlob(void const * blob, uint32_t bsize)
        {
            addUInt32(bsize);
            ensure(bsize);
            memcpy(buf+icursor,blob,bsize);
            icursor += bsize;
        }

        unsigned char * getBlob(uint32_t & mysize)
        {
            mysize = getUInt32();
            unsigned char * ptr = buf+ocursor;
            ocursor += mysize;
            return ptr;
        }

        void addCharSequence(char const * chsbuf, uint32_t chssize)
        {
            ensure(chssize);
            memcpy(buf+icursor,chsbuf,chssize);
            icursor += chssize;
        }

        char * getCharSequence(uint32_t & sizeTillEOB)
        {
            sizeTillEOB = icursor-ocursor;
            unsigned char * ptr = buf+ocursor;
            ocursor = size;
            return reinterpret_cast<char *>(ptr);
        }

        char * getFixedCharSequence(uint32_t mysize)
        {
            unsigned char * ptr = buf+ocursor;
            ocursor += mysize;
            return reinterpret_cast<char *>(ptr);
        }

        void setICursor(uint64_t off)
        {
            icursor = off;
        }

        uint64_t getICursor() const
        {
            return icursor;
        }

        uint64_t getContentSize() const
        {
            return icursor;
        }

        uint64_t getSize() const
        {
            return size;
        }

        void setOCursor(uint64_t off)
        {
            ocursor = off;
        }

        uint64_t getOCursor() const
        {
            return ocursor;
        }

        uint64_t getNRemainingBytes() const
        {
            return icursor-ocursor;
        }

        unsigned char * getPtr()
        {
            return buf;
        }

        unsigned char const * getPtr() const
        {
            return buf;
        }

        uint64_t getSerializedDataSize() const
        {
            return icursor;
        }

        void setAutoDealloc(bool myAutoDealloc)
        {
            autoDealloc = myAutoDealloc;
        }

        bool operator ==(NetworkByteBuffer const & b) const;

        bool operator !=(NetworkByteBuffer const & b) const
        {
            return !(this->operator ==(b));
        }

    protected:
        void ensure(uint64_t extra)
        {
            if(icursor+extra>size)
                resize(extra+icursor-size);
        }

        void resize(uint64_t extra)
        {
            if (!autoDealloc)
                throw std::runtime_error(std::string(
                    "cannot resize fixed buffer"));
            uint64_t maxGrowth = 1ull +
                std::numeric_limits<uint32_t>::max() - size;
            uint64_t growth = extra;
            if (extra > maxGrowth)
                throw std::runtime_error(std::string(
                    "Buffer exceeds maximum size"));
            // testing whether we have to more than double the current  size
            if (extra > size) {
                growth = extra; // grow by the required amount
            } else {
                // we know that we can grow by the necessary amount, but
                // let's see if we can double the size
                if (size > maxGrowth) {
                    // we can't double the size, so let's grow half of that
                    growth = std::max(maxGrowth/2, extra);
                } else {
                    // we can double the size, so let's do it
                    growth = size;
                }
            }
            uint64_t newsize = size + growth;
            unsigned char * newbuf = alloc(newsize);
            if (size>0)
                memcpy(newbuf, buf, size);
            dealloc(buf);
            size = newsize;
            buf = newbuf;
        }

        unsigned char * alloc(uint64_t mysize)
        {
            return new unsigned char[mysize];
        }

        void dealloc(unsigned char * mybuf)
        {
            delete[] mybuf;
        }

    private:
        bool autoDealloc;
        unsigned char * buf;
        uint64_t size;
        uint64_t icursor;
        uint64_t ocursor;
    };

#define BYTE_BUFFER_ADD_OPERATOR(type, method)                    \
    inline NetworkByteBuffer &                                    \
        operator << (NetworkByteBuffer & sbuf, type const & val)  \
    { sbuf.add ## method(val); return sbuf; }                     \
                                                                  \
    inline NetworkByteBuffer &                                    \
        operator >> (NetworkByteBuffer & sbuf, type & val)        \
    { val = sbuf.get ## method(); return sbuf; }                  \

BYTE_BUFFER_ADD_OPERATOR(char, Char)
BYTE_BUFFER_ADD_OPERATOR(bool, Bool)
BYTE_BUFFER_ADD_OPERATOR(int8_t, Int8)
BYTE_BUFFER_ADD_OPERATOR(int16_t, Int16)
BYTE_BUFFER_ADD_OPERATOR(int32_t, Int32)
BYTE_BUFFER_ADD_OPERATOR(int64_t, Int64)
BYTE_BUFFER_ADD_OPERATOR(uint8_t, UInt8)
BYTE_BUFFER_ADD_OPERATOR(uint16_t, UInt16)
BYTE_BUFFER_ADD_OPERATOR(uint32_t, UInt32)
BYTE_BUFFER_ADD_OPERATOR(uint64_t, UInt64)
BYTE_BUFFER_ADD_OPERATOR(float, Float)
BYTE_BUFFER_ADD_OPERATOR(double, Double)
BYTE_BUFFER_ADD_OPERATOR(char *, NTStr)
BYTE_BUFFER_ADD_OPERATOR(char const *, NTStr)
BYTE_BUFFER_ADD_OPERATOR(std::string, STLString)

#undef BYTE_BUFFER_ADD_OPERATOR

template <class T>
inline NetworkByteBuffer & operator << (NetworkByteBuffer & sbuf,
                                        std::vector<T> const & val)
{
    uint32_t size = val.size();
    sbuf << size;
    for (auto const & item : val)
        sbuf << item;
    return sbuf;
}

template <class T>
inline NetworkByteBuffer & operator >> (NetworkByteBuffer & sbuf,
                                        std::vector<T> & val)
{
    uint32_t size;
    sbuf >> size;
    val.reserve(size);
    for (decltype(size) i=0; i<size; ++i) {
        T item;
        sbuf >> item;
        val.push_back(std::move(item));
    }
    return sbuf;
}

template <class T1, class T2>
inline NetworkByteBuffer & operator << (NetworkByteBuffer & sbuf,
                                        std::pair<T1, T2> const & val)
{
    sbuf << val.first;
    sbuf << val.second;
    return sbuf;
}

template <class T1, class T2>
inline NetworkByteBuffer & operator >> (NetworkByteBuffer & sbuf,
                                        std::pair<T1, T2> & val)
{
    sbuf >> val.first;
    sbuf >> val.second;
    return sbuf;
}
} } /* namespace */
