#pragma once

#include <intergdb/expcommon/ExperimentalRun.h>
#include <intergdb/expcommon/ExperimentalData.h>

namespace intergdb { namespace simulation
{

  class VsNumAttributes : public expcommon::ExperimentalRun 
  {
      void process() override;
  private:     
      void makeQueryIOExp(expcommon::ExperimentalData * exp);      
      void makeStorageExp(expcommon::ExperimentalData * exp);
      void makeRunningTimeExp(expcommon::ExperimentalData * exp);
  };


 class VsNumQueryKinds : public expcommon::ExperimentalRun 
  {
      void process() override;
  private:     
      void makeQueryIOExp(expcommon::ExperimentalData * exp);      
      void makeStorageExp(expcommon::ExperimentalData * exp);
      void makeRunningTimeExp(expcommon::ExperimentalData * exp);
  };

 class VsStorageOverheadThreshold : public expcommon::ExperimentalRun 
  {
      void process() override;
  private:     
      void makeQueryIOExp(expcommon::ExperimentalData * exp);      
      void makeStorageExp(expcommon::ExperimentalData * exp);
      void makeRunningTimeExp(expcommon::ExperimentalData * exp);
  };

 class VsQueryLength : public expcommon::ExperimentalRun 
  {
      void process() override;
  private:     
      void makeQueryIOExp(expcommon::ExperimentalData * exp);      
      void makeStorageExp(expcommon::ExperimentalData * exp);
      void makeRunningTimeExp(expcommon::ExperimentalData * exp);
  };

 class VsAttributeSizeSkew : public expcommon::ExperimentalRun 
  {
      void process() override;
  private:     
      void makeQueryIOExp(expcommon::ExperimentalData * exp);      
      void makeStorageExp(expcommon::ExperimentalData * exp);
      void makeRunningTimeExp(expcommon::ExperimentalData * exp);
  };

 class VsQueryFreqSkew : public expcommon::ExperimentalRun 
  {
      void process() override;
  private:     
      void makeQueryIOExp(expcommon::ExperimentalData * exp);      
      void makeStorageExp(expcommon::ExperimentalData * exp);
      void makeRunningTimeExp(expcommon::ExperimentalData * exp);
  };


} } /* namespace */

