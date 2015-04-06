#include <intergdb/simulation/Experiments.h>
#include <cstdlib>

using namespace std;
using namespace intergdb::simulation;



int simulation()
{
    (VsStorageOverheadThreshold()).run();
    (VsNumAttributes()).run();
    (VsNumQueryKinds()).run();
    // (VsQueryLength()).run();
    // (VsAttributeSizeSkew()).run();
    // (VsQueryFreqSkew()).run();    
    return EXIT_SUCCESS;
}


int experiment()
{
    // (VsBlockSize()).run();
    // (VsNumQueryTemplates()).run();
    // (VsTimeDeltaDFS()).run();
    (VsTimeDeltaBFS()).run();
    return EXIT_SUCCESS;
}

int main()
{
    return experiment();
}

