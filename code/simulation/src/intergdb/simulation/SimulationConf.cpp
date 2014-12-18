#include <intergdb/simulation/SimulationConf.h>

#include <iostream>
#include <algorithm>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;
using namespace intergdb::simulation;
using namespace intergdb::util;

constexpr double const SimulationConf::attributeSizes_[numAttributeSizes_];

SimulationConf::SimulationConf() 
 : attributeSizeGen_(getAttributeSizeZipfParam(), numAttributeSizes_),
   queryLengthGen_(getQueryLengthMean(), getQueryLengthStdDev(), 1.0, getAttributeCount()),
   queryTypeFrequencyGen_(getQueryTypeFrequencyZipfParam(), getQueryTypeCount())
{
  unsigned seed = time(NULL);
  attributeSizeGen_.setSeed(seed++);
  queryLengthGen_.setSeed(seed++);
  queryTypeFrequencyGen_.setSeed(seed++);
}

QueryWorkload SimulationConf::getQueryWorkload()
{
  QueryWorkload workload;
  for (size_t i=0, iu=getAttributeCount(); i<iu; ++i)  { 
    double attributeSize = attributeSizes_[attributeSizeGen_.getRandomValue()];
    workload.addAttribute(Attribute(i, attributeSize));
  }
  mt19937 rndGen(time(NULL));
  auto const & attributes = workload.getAttributes();
  vector<size_t> attributeIndices(attributes.size());
  iota(attributeIndices.begin(), attributeIndices.end(), 0);
  double totalFrequency = 0.0;
  for (size_t i=0, iu=getQueryTypeCount(); i<iu; ++i) {
    QuerySummary query;
    size_t queryLength = std::min(static_cast<size_t>(queryLengthGen_.getRandomValue()), attributes.size());
    for (size_t j=0; j<queryLength; ++j) {
      uniform_int_distribution<> udis(0, attributes.size()-j-1);
      size_t k = udis(rndGen);
      query.addAttribute(attributes[attributeIndices[k]]);
      swap(attributeIndices[k], attributeIndices[attributes.size()-j-1]);
    }
    double queryFrequency = queryTypeFrequencyGen_.getItemFrequency(
        queryTypeFrequencyGen_.getRandomValue());
    workload.setFrequency(query, queryFrequency);
    totalFrequency += queryFrequency;
    workload.addQuerySummary(query);
  } 
  for (QuerySummary & query : workload.getQuerySummaries()) 
      workload.setFrequency(query, workload.getFrequency(query)/totalFrequency);
  return workload;
}
