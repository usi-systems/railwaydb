#pragma once

#include <intergdb/simulation/ExperimentalRun.h>
#include <intergdb/simulation/ExperimentalData.h>
#include <intergdb/core/InteractionGraph.h>
#include <intergdb/common/Partitioning.h>

namespace intergdb { namespace simulation
{
    class VsBlockSize : public ExperimentalRun
    {
    public:
        void process() override;

        VsBlockSize();

    private:
        void makeEdgeIOCountExp(ExperimentalData * exp);
        void makeEdgeWriteIOCountExp(ExperimentalData * exp);
        void makeEdgeReadIOCountExp(ExperimentalData * exp);
        void makeRunningTimeExp(ExperimentalData * exp);

    protected:
        void setUp() override;
        void tearDown() override;
        void printTweets();
        std::vector< core::Conf > confs_;
        std::vector< std::unique_ptr<core::InteractionGraph> > graphs_;
        uint64_t tsStart_;
        uint64_t tsEnd_;
        std::unordered_set<int64_t> vertices_;
        double queryZipfParam_ = 0.5;
        int numRuns_ = 50; // Increase by 5 times
        int numQueries_ = 100;
        int blockBufferSize_ = 5 ; /* 5 blocks */
        std::vector<int> blockSizes_ = { 1024, 2048, 4096, 8192, 16384, 32768, 65536 };
    };

    class VsNumQueryTemplates : public ExperimentalRun
    {
    public:
        void process() override;
        VsNumQueryTemplates() { }
    private:
        void makeEdgeIOCountExp(ExperimentalData * exp);
        void makeEdgeWriteIOCountExp(ExperimentalData * exp);
        void makeEdgeReadIOCountExp(ExperimentalData * exp);
        void makeRunningTimeExp(ExperimentalData * exp);
        std::unique_ptr<core::Conf> conf_;
        std::unique_ptr<core::InteractionGraph> graph_;
        uint64_t tsStart_;
        uint64_t tsEnd_;
        std::unordered_set<int64_t> vertices_;
        int numRuns_ = 10;
        int blockSize_ = 65536;
        int blockBufferSize_ = 5 ;
        std::vector<int> queryTemplatesSizes_ = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        double queryZipfParam_ = 0.5;
        int numQueries_ = 100;

    protected:
        void setUp() override ;
        void tearDown() override { };
    };



 class VsTimeDeltaDFS : public ExperimentalRun
    {
    public:
        void process() override;

        VsTimeDeltaDFS();

    private:
        void makeEdgeIOCountExp(ExperimentalData * exp);
        void makeEdgeWriteIOCountExp(ExperimentalData * exp);
        void makeEdgeReadIOCountExp(ExperimentalData * exp);
        void makeRunningTimeExp(ExperimentalData * exp);

    protected:
        void setUp() override;
        void tearDown() override;
        std::unique_ptr<core::Conf> conf_;
        std::unique_ptr<core::InteractionGraph> graph_;
        uint64_t tsStart_;
        uint64_t tsEnd_;
        std::unordered_set<int64_t> vertices_;
        int numRuns_ = 10;
        int blockSize_ = 65536;
        int blockBufferSize_ = 5 ;
        int numQueries_ = 100;
        int numQueryTemplates_ = 3;
        double queryZipfParam_ = 0.5;
        std::vector<double> timeDeltas_ = { 0.10, 0.20, 0.30, 0.40, 0.50, 0.60, 0.70, 0.80, 0.90, 1.00 };
    };

 class VsTimeDeltaBFS : public ExperimentalRun
    {
    public:
        void process() override;

        VsTimeDeltaBFS();

    private:
        void makeEdgeIOCountExp(ExperimentalData * exp);
        void makeEdgeWriteIOCountExp(ExperimentalData * exp);
        void makeEdgeReadIOCountExp(ExperimentalData * exp);
        void makeRunningTimeExp(ExperimentalData * exp);

    protected:
        void setUp() override;
        void tearDown() override;
        std::unique_ptr<core::Conf> conf_;
        std::unique_ptr<core::InteractionGraph> graph_;
        uint64_t tsStart_;
        uint64_t tsEnd_;
        std::unordered_set<int64_t> vertices_;
        int numRuns_ = 10;
        int blockSize_ = 65536;
        int blockBufferSize_ = 5 ;
        int numQueries_ = 100;
        int numQueryTemplates_ = 3;
        double queryZipfParam_ = 0.5;
        std::vector<double> timeDeltas_ = { 0.10, 0.20, 0.30, 0.40, 0.50, 0.60, 0.70, 0.80, 0.90, 1.00 };
    };


    class VsNumAttributes : public ExperimentalRun
    {
    public:
        void process() override;

    private:
        void makeQueryIOExp(ExperimentalData * exp);

        void makeStorageExp(ExperimentalData * exp);

        void makeRunningTimeExp(ExperimentalData * exp);
    };


    class VsNumQueryKinds : public ExperimentalRun
    {
    public:
        void process() override;

    private:
        void makeQueryIOExp(ExperimentalData * exp);

        void makeStorageExp(ExperimentalData * exp);

        void makeRunningTimeExp(ExperimentalData * exp);
    };

    class VsStorageOverheadThreshold : public ExperimentalRun
    {
    public:
        void process() override;

    private:
        void makeQueryIOExp(ExperimentalData * exp);

        void makeStorageExp(ExperimentalData * exp);

        void makeRunningTimeExp(ExperimentalData * exp);
    };

    class VsQueryLength : public ExperimentalRun
    {
    public:
        void process() override;

    private:
        void makeQueryIOExp(ExperimentalData * exp);

        void makeStorageExp(ExperimentalData * exp);

        void makeRunningTimeExp(ExperimentalData * exp);
    };

    class VsAttributeSizeSkew : public ExperimentalRun
    {
    public:
        void process() override;

    private:
        void makeQueryIOExp(ExperimentalData * exp);

        void makeStorageExp(ExperimentalData * exp);

        void makeRunningTimeExp(ExperimentalData * exp);
    };

    class VsQueryFreqSkew : public ExperimentalRun
    {
    public:
        void process() override;

    private:
        void makeQueryIOExp(ExperimentalData * exp);

        void makeStorageExp(ExperimentalData * exp);

        void makeRunningTimeExp(ExperimentalData * exp);
    };

    class VsInteractionCount : public ExperimentalRun
    {
    public:
        void process() override;
    };

} } /* namespace */
