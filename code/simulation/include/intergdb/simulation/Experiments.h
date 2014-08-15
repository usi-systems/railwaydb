#pragma once

#include <intergdb/simulation/ExperimentalRun.h>

namespace intergdb { namespace simulation
{
  class SampleExperiment : public ExperimentalRun 
  {
    void process() override;
  };

 class RunningTimeVsNumAttributes : public ExperimentalRun 
  {
    void process() override;
  };

 class QueryIOVsNumAttributes : public ExperimentalRun 
  {
    void process() override;
  };


  // List other experiments here

} } /* namespace */

