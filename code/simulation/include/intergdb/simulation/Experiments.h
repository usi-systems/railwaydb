#pragma once

#include <intergdb/simulation/ExperimentalRun.h>

namespace intergdb { namespace simulation
{

  class ExperimentalData;

  class SampleExperiment : public ExperimentalRun 
  {
    void process() override;
  };

  class VsNumAttributes : public ExperimentalRun 
  {
      void process() override;
  private:     
      void makeQueryIOExp(ExperimentalData * exp);      
      void makeStorageExp(ExperimentalData * exp);
      void makeRunningTimeExp(ExperimentalData * exp);
  };

 class RunningTimeVsNumQueryKinds : public ExperimentalRun 
  {
    void process() override;
  };

} } /* namespace */

