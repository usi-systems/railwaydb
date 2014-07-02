#pragma once

#include <string>

namespace intergdb { namespace simulation
{
    class ExperimentalRun {
    public:
        virtual ~ExperimentalRun() {}
        virtual void process() = 0;
        void run();
    protected:
        std::string getClassName();
        std::string getFullyQualifiedClassName();
    };

} } /* namespace */

