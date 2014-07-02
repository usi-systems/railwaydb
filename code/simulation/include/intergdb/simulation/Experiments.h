#pragma once

#include <intergdb/simulation/ExperimentalRun.h>

namespace intergdb { namespace simulation
{
  class SampleExperiment : public ExperimentalRun 
  {
    void process() override;
  };

  // List other experiments here

} } /* namespace */

