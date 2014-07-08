#pragma once

#include <cstdlib>
#include <string>

namespace intergdb { namespace common
{
  class Attribute
  {
  public:
    Attribute() {}
    Attribute(size_t index, double size) : index_(index), size_(size) {}
    size_t getIndex() const { return index_; }
    void setIndex(size_t index) { index_ = index; }
    double getSize() const { return size_; }
    void setSize(double size) { size_ = size; }
    std::string toString() const;
  private:
    size_t index_;
    double size_;
  };
} }