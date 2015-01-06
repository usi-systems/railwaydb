#pragma once

#include <cstdlib>
#include <string>
#include <iostream>

namespace intergdb { namespace common
{

  #define SIZE_COMPUTED -1

  class Attribute
  {
  private:
    static char const * dataTypeStrings_[];
  public:
    enum DataType { INT64, DOUBLE, STRING, UNDEFINED };
    Attribute() { }
    Attribute(size_t index, std::string name, DataType type)
      : index_(index), size_(SIZE_COMPUTED), name_(name), type_(type) {}

    Attribute(size_t index, double size) 
        : index_(index), size_(size), name_(""), type_(UNDEFINED) {}

    Attribute(Attribute const & other ) 
      : index_(other.index_), size_(other.size_), name_(other.name_), type_(other.type_) {}
    size_t getIndex() const { return index_; }
    void setIndex(size_t index) { index_ = index; }
    std::string getName() const { return name_; }
    double getSize() const { std::cout << "Attribute::getSize() warning! size not set correctly" << std::endl; return size_; }
    DataType getType() const { return type_; }
    std::string toString() const;
    static char const * getTypeString(DataType type) { return dataTypeStrings_[type]; }
  private:
    size_t index_;
    double size_;
    std::string name_;
    DataType type_;
  };
} }
