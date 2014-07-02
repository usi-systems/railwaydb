#ifndef INTERGDB_SIMULATION_EXPERIMENTALRUN_H
#define INTERGDB_SIMULATION_EXPERIMENTALRUN_H

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

#endif /* INTERGDB_SIMULATION_EXPERIMENTALRUN_H */
