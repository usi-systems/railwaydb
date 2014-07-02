#include <intergdb/simulation/AutoTimer.h>

using namespace intergdb::simulation;
using namespace boost::chrono;

AutoTimer::AutoTimer()
{}

AutoTimer::AutoTimer(bool start)
{
  if (start) 
    time_ = process_cpu_clock::now(); 
}

void AutoTimer::start()
{
  time_ = process_cpu_clock::now(); 
}

void AutoTimer::stop()
{
  end_ = process_cpu_clock::now();  
}

double AutoTimer::getCPUTimeInSeconds()
{
  process_cpu_clock::times timeTaken = (end_ - time_).count();
  process_user_cpu_clock::duration userTimeTaken(timeTaken.user);
  process_system_cpu_clock::duration systemTimeTaken(timeTaken.system);
  return duration<double>(userTimeTaken).count() + 
         duration<double>(systemTimeTaken).count();
}

double AutoTimer::getRealTimeInSeconds()
{
  process_cpu_clock::times timeTaken = (end_ - time_).count();
  process_real_cpu_clock::duration realTimeTaken(timeTaken.real);
  return duration<double>(realTimeTaken).count();
}

double AutoTimer::getCPUUtilization()
{
  process_cpu_clock::times timeTaken = (end_ - time_).count();
  process_user_cpu_clock::duration userTimeTaken(timeTaken.user);
  process_system_cpu_clock::duration systemTimeTaken(timeTaken.system);
  process_real_cpu_clock::duration realTimeTaken(timeTaken.real);
  return (duration<double>(userTimeTaken).count() + 
      duration<double>(systemTimeTaken).count()) / 
         duration<double>(realTimeTaken).count();
}

