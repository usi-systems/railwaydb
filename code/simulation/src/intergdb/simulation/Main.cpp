#include <intergdb/simulation/Experiments.h>

#include <cstdlib>

using namespace std;
using namespace intergdb;
using namespace intergdb::simulation;

int main()
{
    (QueryIOVsNumAttributes()).run();
    (SampleExperiment()).run();
    return EXIT_SUCCESS;
}

