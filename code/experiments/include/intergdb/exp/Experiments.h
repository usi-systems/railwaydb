#pragma once

#include <intergdb/expcommon/ExperimentalRun.h>
#include <intergdb/expcommon/ExperimentalData.h>
#include <intergdb/core/InteractionGraph.h>
#include <intergdb/common/Partitioning.h>

namespace intergdb { namespace exp
{

  class VsBlockSize : public expcommon::ExperimentalRun 
  {
      void process(); //override;
  private:          
      void makeQueryIOExp(expcommon::ExperimentalData * exp);      
      void makeStorageExp(expcommon::ExperimentalData * exp);
      void makeRunningTimeExp(expcommon::ExperimentalData * exp);
      void runWorkload(core::InteractionGraph * graph);
      void createGraph();
      core::TimeSlicedPartitioning convertPartitioning(intergdb::common::Partitioning p);

  protected:
      void setUp();    //override;
      void tearDown(); //override;
      std::unique_ptr<core::Conf> conf;
      std::unique_ptr<core::InteractionGraph> graph;
  };


} } /* namespace */

