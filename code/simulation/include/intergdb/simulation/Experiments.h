#ifndef INTERGDB_SIMULATION_EXPERIMENTS_H
#define INTERGDB_SIMULATION_EXPERIMENTS_H

#include <intergdb/simulation/ExperimentalRun.h>

namespace intergdb { namespace simulation
{
  class SampleExperiment : public ExperimentalRun 
  {
    void process() override;
  };

  // List other experiments here

} } /* namespace */

#endif /* INTERGDB_SIMULATION_EXPERIMENTS_H */
