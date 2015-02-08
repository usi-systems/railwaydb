#pragma once

#include <string>

namespace intergdb { namespace expcommon
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

