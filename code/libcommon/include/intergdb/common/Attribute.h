#pragma once

#include <cstdlib>
#include <string>

namespace intergdb { namespace common
{
  class Attribute
  {
  public:

    enum DataType { INT64, DOUBLE, STRING };

    Attribute() {}
    Attribute(size_t index, double size, std::string name, DataType type)
        : index_(index), size_(size), name_(name), type_(type) {}


    Attribute(size_t index, double size) : index_(index), size_(size) {}
    size_t getIndex() const { return index_; }
    void setIndex(size_t index) { index_ = index; }
    double getSize() const { return size_; }
    std::string toString() const;
  private:
    size_t index_;
    double size_;
    std::string name_;
    DataType type_;
  };
} }
