#pragma once

#include <intergdb/simulation/ExperimentalRun.h>
#include <intergdb/simulation/ExperimentalData.h>
#include <intergdb/core/InteractionGraph.h>
#include <intergdb/common/Partitioning.h>

namespace intergdb { namespace simulation
    {

        /////////////////// Twitter Experiments ///////////////////

        class VsBlockSize : public ExperimentalRun 
        {
            void process(); //override;
        private:                      
            void makeEdgeIOCountExp(ExperimentalData * exp);      
            void makeEdgeWriteIOCountExp(ExperimentalData * exp);
            void makeEdgeReadIOCountExp(ExperimentalData * exp);
            void runWorkload(core::InteractionGraph * graph, std::vector<core::FocusedIntervalQuery> & queries, std::vector<int> indicies);
            std::vector<int> genWorkload(size_t numQueryTypes);
    
        protected:
            void setUp();    //override;
            void tearDown(); //override;
            void printTweets();
            std::vector< core::Conf > confs_;
            std::vector< std::unique_ptr<core::InteractionGraph> > graphs_;
            uint64_t tsStart_;
            uint64_t tsEnd_;
            std::unordered_set<int64_t> vertices_;
            double queryZipfParam_ = 0.5;
            int numRuns_ = 2;
            int numQueries_ = 100;
            int blockBufferSize_ = 5; /* 5 blocks */
            std::vector<int> blockSizes_ = { 1, 2, 4, 8, 16, 32, 64 };
        public:
            VsBlockSize();

        };

    
        /////////////////// Simulation Experiments ///////////////////

        class VsNumAttributes : public ExperimentalRun 
        {
            void process() override;
        private:     
            void makeQueryIOExp(ExperimentalData * exp);      
            void makeStorageExp(ExperimentalData * exp);
            void makeRunningTimeExp(ExperimentalData * exp);
        };


        class VsNumQueryKinds : public ExperimentalRun 
        {
            void process() override;
        private:     
            void makeQueryIOExp(ExperimentalData * exp);      
            void makeStorageExp(ExperimentalData * exp);
            void makeRunningTimeExp(ExperimentalData * exp);
        };

        class VsStorageOverheadThreshold : public ExperimentalRun 
        {
            void process() override;
        private:     
            void makeQueryIOExp(ExperimentalData * exp);      
            void makeStorageExp(ExperimentalData * exp);
            void makeRunningTimeExp(ExperimentalData * exp);
        };

        class VsQueryLength : public ExperimentalRun 
        {
            void process() override;
        private:     
            void makeQueryIOExp(ExperimentalData * exp);      
            void makeStorageExp(ExperimentalData * exp);
            void makeRunningTimeExp(ExperimentalData * exp);
        };

        class VsAttributeSizeSkew : public ExperimentalRun 
        {
            void process() override;
        private:     
            void makeQueryIOExp(ExperimentalData * exp);      
            void makeStorageExp(ExperimentalData * exp);
            void makeRunningTimeExp(ExperimentalData * exp);
        };

        class VsQueryFreqSkew : public ExperimentalRun 
        {
            void process() override;
        private:     
            void makeQueryIOExp(ExperimentalData * exp);      
            void makeStorageExp(ExperimentalData * exp);
            void makeRunningTimeExp(ExperimentalData * exp);
        };


    } } /* namespace */

