#include <intergdb/simulation/Experiments.h>

#include <cstdlib>

using namespace std;
using namespace intergdb;
using namespace intergdb::simulation;

int main()
{
    //(VsStorageOverheadThreshold()).run();
    //(VsNumAttributes()).run();
    (VsNumQueryKinds()).run();
    return EXIT_SUCCESS;
}

