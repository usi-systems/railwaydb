#pragma once

#include <intergdb/expcommon/ExperimentalRun.h>
#include <intergdb/expcommon/ExperimentalData.h>
#include <intergdb/core/InteractionGraph.h>
#include <intergdb/common/Partitioning.h>

namespace intergdb { namespace simulation
    {

        /////////////////// Twitter Experiments ///////////////////

        class VsBlockSize : public expcommon::ExperimentalRun 
        {
            void process(); //override;
        private:          
            void makeQueryIOExp(expcommon::ExperimentalData * exp);      
            void makeStorageExp(expcommon::ExperimentalData * exp);
            void makeRunningTimeExp(expcommon::ExperimentalData * exp);
            void runWorkload(core::InteractionGraph * graph);
            void createGraph();
        protected:
            void setUp();    //override;
            void tearDown(); //override;
            std::unique_ptr<core::Conf> conf;
            std::unique_ptr<core::InteractionGraph> graph;
        };

    
        /////////////////// Simulation Experiments ///////////////////

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

