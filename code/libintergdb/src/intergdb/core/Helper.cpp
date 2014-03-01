#include <intergdb/core/Helper.h>

#include <intergdb/core/Types.h>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace intergdb::core;
using namespace boost::posix_time;
using namespace boost::gregorian;

uint64_t Helper::getCurrentTimemillis()
{
    ptime epoch(date(1970,1,1));
    ptime now = microsec_clock::local_time();
    time_duration diff = now - epoch;
    return diff.total_milliseconds();
}

Timestamp Helper::getCurrentTimestamp()
{
    ptime epoch(date(1970,1,1));
    ptime now = microsec_clock::local_time();
    time_duration diff = now - epoch;
    uint64_t msec = diff.total_milliseconds();
    diff = diff - milliseconds(msec);
    uint64_t nsec = diff.total_nanoseconds();
    Timestamp t = msec;
    t *= 1000*1000;
    t += nsec;
    return t;
}

Timestamp Helper::millisToTimestamp(double millis)
{
    Timestamp t = millis;
    t *= 1000*1000;
    t += (millis-((uint64_t)millis))*1000*1000;
    return t;
}

std::string Helper::exec(std::string cmd) {
    FILE* pipe = ::popen(cmd.c_str(), "r");
    if (!pipe)
        return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!::feof(pipe)) {
        if(::fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    ::pclose(pipe);
    return result;
}
