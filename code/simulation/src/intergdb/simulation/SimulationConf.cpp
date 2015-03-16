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
    // double totalFrequency = 0.0;
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
    // TODO(rjs): generate the times
    
    std::vector<core::FocusedIntervalQuery> queries;

    // Query temporal range will all be the same "30 min. or 1 hour"
    // A single bucket over that time range
    // Random numbers scattered through the range
    // Random for size
    // Uniformly random size of location

    // used to get a random time for the interval query
    size_t queryTimeMean = (tsStart + tsEnd) / 2;
    double queryTimeStdDev = queryTimeMean - tsStart;
    NormalRand queryTimeGen(queryTimeMean, queryTimeStdDev, tsStart, tsEnd);

    // used to get a random start node for the interval query
    size_t vertexIdMean = (vertices.size()) / 2;
    double vertexIdStdDev = vertexIdMean - 1;
    NormalRand vertexIdGen(vertexIdMean, vertexIdStdDev, 1.0, vertices.size());
    
    uint64_t start;
    uint64_t end;
    uint64_t tmp;

    // Create 1 random query for each set of attributeNames
    for (auto attributeNames : queryAttributeNames) {
        start = queryTimeGen.getRandomValue();
        end = queryTimeGen.getRandomValue();
        if (end < start) {
            tmp = start;
            start = end;
            end = tmp;
        } 
        queries.push_back(FocusedIntervalQuery(vertexIdGen.getRandomValue(), start, end, attributeNames));
    }
    
    return queries;
}
