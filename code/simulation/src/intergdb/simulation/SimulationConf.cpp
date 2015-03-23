#include <intergdb/simulation/SimulationConf.h>
#include <intergdb/common/SchemaStats.h>

#include <iostream>
#include <algorithm>

using namespace std;
using namespace intergdb;
using namespace intergdb::core;
using namespace intergdb::common;
using namespace intergdb::simulation;
using namespace intergdb::util;

constexpr double const SimulationConf::attributeSizes_[numAttributeSizes_];


SimulationConf::SimulationConf() 
 : attributeSizeGen_(getAttributeSizeZipfParam(), numAttributeSizes_),
   queryLengthGen_(getQueryLengthMean(), getQueryLengthStdDev(), 1.0, getAttributeCount()),
   queryTypeFrequencyGen_(getQueryTypeFrequencyZipfParam(), getQueryTypeCount()),
   queryTimeGen_(getQueryTimeZipfParam(), getQueryTypeCount()) // TODO(rjs) : should we have a numQueries?
{
  unsigned seed = time(NULL);
  attributeSizeGen_.setSeed(seed++);
  queryLengthGen_.setSeed(seed++);
  queryTypeFrequencyGen_.setSeed(seed++);
}

std::pair<common::QueryWorkload, common::SchemaStats> SimulationConf::getQueryWorkloadAndStats()
{
  QueryWorkload workload;
  SchemaStats stats;
  for (size_t i=0, iu=getAttributeCount(); i<iu; ++i)  { 
    double attributeSize = attributeSizes_[attributeSizeGen_.getRandomValue()];
    workload.addAttribute(Attribute(i, std::to_string(i), common::Attribute::UNDEFINED));
    stats.incrCountAndBytes(i, attributeSize);
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
  return std::make_pair(workload, stats);
}

std::vector<core::FocusedIntervalQuery> SimulationConf::getQueries(InteractionGraph * graph, 
                                                                   uint64_t& tsStart, 
                                                                   uint64_t& tsEnd,
                                                                   std::unordered_set<int64_t> vertices)
{
    mt19937 rndGen(time(NULL));
    auto const & attributes = graph->getConf().getEdgeSchema().getAttributes();
    vector<size_t> attributeIndices(attributes.size());
    iota(attributeIndices.begin(), attributeIndices.end(), 0);
    vector<std::vector<std::string> > queryAttributeNames;
    for (size_t i=0, iu=getQueryTypeCount(); i<iu; ++i) {        
        std::vector<std::string> attributeNames;
        size_t queryLength = std::min(static_cast<size_t>(queryLengthGen_.getRandomValue()), attributes.size());
        for (size_t j=0; j<queryLength; ++j) {
            uniform_int_distribution<> udis(0, attributes.size()-j-1);
            size_t k = udis(rndGen);       
            attributeNames.push_back( attributes[attributeIndices[k]].getName());          
            swap(attributeIndices[k], attributeIndices[attributes.size()-j-1]);
        }
        queryAttributeNames.push_back(attributeNames);        
    }
    
    std::vector<core::FocusedIntervalQuery> queries;

    std::vector<int64_t> vertexList;
    std::copy(vertices.begin(), vertices.end(), std::back_inserter(vertexList));


    // used to get a random start node for the interval query
    size_t vertexIdMean = (vertices.size()) / 2;
    double vertexIdStdDev = vertexIdMean - 1;
    NormalRand vertexIdGen(vertexIdMean, vertexIdStdDev, 1.0, vertices.size());

    std::cout << "random vertices" << std::endl;
    // Create 1 random query for each set of attributeNames
    for (auto attributeNames : queryAttributeNames) {
        std::cout << vertexList[vertexIdGen.getRandomValue()] << std::endl;
        queries.push_back(FocusedIntervalQuery(vertexList[vertexIdGen.getRandomValue()], 
                                               tsStart,  
                                               tsEnd,
                                               attributeNames));
    }
    std::cout << "done random vertices" << std::endl;

    return queries;
}
