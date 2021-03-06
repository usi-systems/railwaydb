#pragma once

#include <intergdb/common/Types.h>
#include <intergdb/common/Attribute.h>
#include <intergdb/core/AttributeData.h>
#include <intergdb/core/Schema.h>

#include <stdexcept>
#include <typeinfo>
#include <type_traits>
#include <string>

using namespace intergdb::common;

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
        throw std::runtime_error("Type "+std::string(typeid(T).name()) +
            " does not have getSerializedSizeOf defined for it");
        return 0;
    }

    inline size_t getSerializedSizeOf(std::string const & str)
    {
        return sizeof(uint32_t) + str.size();
    }

    inline size_t getSerializedSizeOf(AttributeData const & data)
    {
        size_t size = 0;
        Schema const & schema = data.getSchema();
        for (auto const & indexTypePair : data.getFields()) {
            switch (schema.getAttributes().at(indexTypePair.first).getType()) {
                case Attribute::INT64:
                {
                    size += getSerializedSizeOf(
                        data.getIntAttribute(indexTypePair.first));
                    break;
                }
                case Attribute::DOUBLE:
                {
                    size += getSerializedSizeOf(
                        data.getDoubleAttribute(indexTypePair.first));
                    break;
                }
                case Attribute::STRING:
                {
                    size += getSerializedSizeOf(
                        data.getStringAttribute(indexTypePair.first));
                    break;
                }
                default:
                    assert(false);
                }
        }
        return size;
    }
} } /* namespace */
