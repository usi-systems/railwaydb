#include <intergdb/simulation/Experiments.h>

#include <cstdlib>

using namespace std;
using namespace intergdb;
using namespace intergdb::simulation;

int main()
{
    (RunningTimeVsNumAttributes()).run();
    //(QueryIOVsNumAttributes()).run();
    //(StorageOverheadVsNumAttributes()).run();
    return EXIT_SUCCESS;
}

