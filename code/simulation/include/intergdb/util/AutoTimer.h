#pragma once

#include <boost/chrono.hpp>

namespace intergdb { namespace util
{
    class AutoTimer
    {
    public:
        AutoTimer();
        AutoTimer(bool start);
        void start();
        void stop();
        double getCPUTimeInSeconds();
        double getRealTimeInSeconds();
        double getCPUUtilization();
    private:
        boost::chrono::process_cpu_clock::time_point time_;
        boost::chrono::process_cpu_clock::time_point end_;
    };
}}

