#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

namespace intergdb { namespace common
{
    class Attribute
    {
    public:
        enum DataType { INT64, DOUBLE, STRING, UNDEFINED };

        Attribute()
        {}

        Attribute(size_t index, std::string name, DataType type)
            : index_(index), name_(name), type_(type) {}

        Attribute(Attribute const & other )
            : index_(other.index_), name_(other.name_), type_(other.type_)
        {}

        size_t getIndex() const
        {
            return index_;
        }

        void setIndex(size_t index)
        {
            index_ = index;
        }

        std::string getName() const
        {
            return name_;
        }

        DataType getType() const
        {
            return type_;
        }

        std::string toString() const;

        static char const * getTypeString(DataType type)
        {
            return dataTypeStrings_[type];
        }

    private:
        static char const * dataTypeStrings_[];
        size_t index_;
        std::string name_;
        DataType type_;
    };
} }
