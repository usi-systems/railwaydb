#include <intergdb/simulation/Experiments.h>
#include <cstdlib>

using namespace std;
using namespace intergdb::simulation;

void simulation()
{
    // (VsStorageOverheadThreshold()).run();
    // (VsNumAttributes()).run();
    // (VsNumQueryKinds()).run();
    // (VsQueryLength()).run();
    // (VsAttributeSizeSkew()).run();
    // (VsQueryFreqSkew()).run();
}

void experiment()
{
    // (VsInteractionCount()).run();
    // (VsBlockSize()).run();
    // (VsNumQueryTemplates()).run();
    (VsTimeDeltaDFS()).run();
    // (VsTimeDeltaBFS()).run();
}

int main()
{
  simulation();
  experiment();
  return 0;
}

