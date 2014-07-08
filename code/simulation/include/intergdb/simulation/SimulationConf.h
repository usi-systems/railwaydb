#pragma once

#include <intergdb/common/QueryWorkload.h>

namespace intergdb { namespace simulation
{
  class SimulationConf
  {
  public:
    void setAttributeCount(size_t attributeCount) 
      { attributeCount_ = attributeCount; }
    size_t getAttributeCount() const
      { return attributeCount_; }
    void setAttributeSizeZipfParam(double attributeSizeZipfParam) 
      { attributeSizeZipfParam_ = attributeSizeZipfParam; }
    double getAttributeSizeZipfParam() const
      { return attributeSizeZipfParam_; }
    void setQueryLengthMean(size_t queryLengthMean) 
      { queryLengthMean_ = queryLengthMean; }
    size_t getQueryLengthMean() const
      { return queryLengthMean_; }
    void setQueryLengthStdDev(double queryLengthStdDev) 
      { queryLengthStdDev_ = queryLengthStdDev; }
    double getQueryLengthStdDev() const
      { return queryLengthStdDev_; }
    void setQueryTypeCount(size_t queryTypeCount) 
      { queryTypeCount_ = queryTypeCount; }
    size_t getQueryTypeCount() const
      { return queryTypeCount_; }
    void setQueryTypeFrequencyZipfParam(double queryTypeFrequencyZipfParam) 
      { queryTypeFrequencyZipfParam_ = queryTypeFrequencyZipfParam; }
    double getQueryTypeFrequencyZipfParam() const
      { return queryTypeFrequencyZipfParam_; }
    common::QueryWorkload getQueryWorkload();
  private:
    size_t attributeCount_ = 10;
    double attributeSizeZipfParam_ = 0.5;
    size_t queryLengthMean_ = 3;
    double queryLengthStdDev_ = 2.0;
    size_t queryTypeCount_ = 5;
    double queryTypeFrequencyZipfParam_ = 0.5;
  };

} } /* namespace */

