
#ifndef INTERGDB_SIMULATION_AUTOTIMER_H
#define INTERGDB_SIMULATION_AUTOTIMER_H

#include <boost/chrono.hpp>

namespace intergdb { namespace simulation
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

#endif /* INTERGDB_SIMULATION_AUTOTIMER_H */
