#pragma once

#include <intergdb/expcommon/ExperimentalRun.h>
#include <intergdb/expcommon/ExperimentalData.h>
#include <intergdb/core/InteractionGraph.h>

namespace intergdb { namespace exp
{

  class VsBlockSize : public expcommon::ExperimentalRun 
  {
      void process(); //override;
  public:          
      void setUp();
      void tearDown();
  private:          
      void makeQueryIOExp(expcommon::ExperimentalData * exp);      
      void makeStorageExp(expcommon::ExperimentalData * exp);
      void makeRunningTimeExp(expcommon::ExperimentalData * exp);
      void test_graph(core::InteractionGraph * graph);

  protected:
      std::unique_ptr<core::Conf> conf;
      std::unique_ptr<core::InteractionGraph> graph;
  };


} } /* namespace */

