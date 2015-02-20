#include <intergdb/simulation/Experiments.h>
#include <cstdlib>

using namespace std;
//using namespace intergdb;
//using namespace intergdb::core;
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
    // printTweets();
    //createTweetDB();
    (VsBlockSize()).run();
    return EXIT_SUCCESS;
}

int main()
{
    return experiment();
}

