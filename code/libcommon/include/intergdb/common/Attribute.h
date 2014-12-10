#pragma once

#include <cstdlib>
#include <string>

namespace intergdb { namespace common
{
  class Attribute
  {
  public:

    enum DataType { INT64, DOUBLE, STRING };
    // char const * dataTypesStrings[] = { "INT64", "DOUBLE", "STRING" };

    Attribute(size_t index, double size, std::string name, DataType type)
        : index_(index), size_(size), name_(name), type_(type) {}


    Attribute(size_t index, double size) : index_(index), size_(size) {}
    size_t getIndex() const { return index_; }
    void setIndex(size_t index) { index_ = index; }
    std::string getName() const { return name_; }
    double getSize() const { return size_; }
    DataType getType() const { return type_; }
    std::string toString() const;
  private:
    size_t index_;
    double size_;
    std::string name_;
    DataType type_;
  };
} }
