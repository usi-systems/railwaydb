#include <intergdb/simulation/SimulationConf.h>

#include <intergdb/util/ZipfRand.h>
#include <intergdb/util/NormalRand.h>

using namespace std;
using namespace intergdb;
using namespace intergdb::common;
using namespace intergdb::simulation;
using namespace intergdb::util;

QueryWorkload SimulationConf::getQueryWorkload()
{
  unsigned seed = time(NULL);
  QueryWorkload workload;
  ZipfRand attributeSizeGen(getAttributeSizeZipfParam(), getAttributeCount());
  attributeSizeGen.setSeed(seed);
  NormalRand queryLengthGen(getQueryLengthMean(), getQueryLengthStdDev(), 1.0, getAttributeCount());
  queryLengthGen.setSeed(seed);
  ZipfRand queryTypeFrequencyGen(getQueryTypeFrequencyZipfParam(), getQueryTypeCount());
  queryTypeFrequencyGen.setSeed(seed);

  double totalSize = 0.0;
  for (size_t i=0, iu=getAttributeCount(); i<iu; ++i)  { 
    double attributeSize = attributeSizeGen.getItemFrequency(
      attributeSizeGen.getRandomValue());
    workload.addAttribute(Attribute(i, attributeSize));
    totalSize += attributeSize;
  }
  for (Attribute & attribute : workload.getAttributes()) 
    attribute.setSize(attribute.getSize()/totalSize);
  mt19937 rndGen(seed);
  auto const & attributes = workload.getAttributes();
  vector<size_t> attributeIndices(attributes.size());
  iota(attributeIndices.begin(), attributeIndices.end(), 0);
  double totalFrequency = 0.0;
  for (size_t i=0, iu=getQueryTypeCount(); i<iu; ++i) {
    Query query;
    size_t queryLength = queryLengthGen.getRandomValue();
    for (size_t j=0; j<queryLength; ++j) {
      uniform_int_distribution<> udis(0, attributes.size()-j-1);
      size_t k = udis(rndGen);
      query.addAttribute(attributes[attributeIndices[k]]);
      swap(attributeIndices[k], attributeIndices[attributes.size()-j-1]);
    }
    double queryFrequency = queryTypeFrequencyGen.getItemFrequency(
        queryTypeFrequencyGen.getRandomValue());
    query.setFrequency(queryFrequency);
    totalFrequency += queryFrequency;
    workload.addQuery(query);
  } 
  for (Query & query : workload.getQueries()) 
    query.setFrequency(query.getFrequency()/totalFrequency);
  return workload;
}