#pragma once

#include <vector>
#include <algorithm>
#include <boost/functional/hash.hpp>

namespace intergdb { namespace common
{
  class Attribute;
  class QuerySummary
  {
  public:
     QuerySummary() {}


    QuerySummary(std::vector<Attribute const *> const & attributes) 
      : attributes_(attributes)
    {
        std::sort(attributes_.begin(), attributes_.end());
        hash_ = hash();
    } 

    bool operator==( const QuerySummary& other) const
    {        
        if (attributes_.size() != other.attributes_.size()) {
            return false;
        }
        int i = 0;
        for (auto mine : attributes_) {
            if (mine != other.attributes_[i]) 
                return false;
            i++;
        }      
        return true;
    }

    void addAttribute(Attribute const & attribute) { attributes_.push_back(&attribute); }
    void setAttributes(std::vector<Attribute const *> const & attributes) { attributes_ = attributes; }
    std::vector<Attribute const *> const & getAttributes() const { return attributes_; }

    std::string toString() const;
    size_t getHashCode() const {
        return hash_;
    }
  private:
    std::vector<Attribute const *> attributes_;
    size_t hash_;    
    std::size_t hash() {
        using boost::hash_value;
        using boost::hash_combine;        
        std::size_t seed = 0;
        for (auto attr : attributes_) {
            hash_combine(seed,hash_value(attr));
        }
        return seed;
    }
  };
} } /* namespace */

namespace std { 
    template<>
        struct hash<intergdb::common::QuerySummary> {
        inline std::size_t operator()(const intergdb::common::QuerySummary& q) const
        {
            return q.getHashCode();
        }        

    };
}
