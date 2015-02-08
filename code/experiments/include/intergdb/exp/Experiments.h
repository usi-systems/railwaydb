#pragma once

#include <intergdb/expcommon/ExperimentalRun.h>
#include <intergdb/expcommon/ExperimentalData.h>

namespace intergdb { namespace experiments
{

  class VsBlockSize : public expcommon::ExperimentalRun 
  {
      void process(); //override;
  private:     
      void makeQueryIOExp(expcommon::ExperimentalData * exp);      
      void makeStorageExp(expcommon::ExperimentalData * exp);
      void makeRunningTimeExp(expcommon::ExperimentalData * exp);
  };


} } /* namespace */

