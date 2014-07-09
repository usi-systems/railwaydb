
#include <cstdlib>
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif
#include "solver.h"
#ifdef __cplusplus
}
#endif

using namespace std;

extern "C" int solve(int argc, char *argv[]);


int main()
{
    cout << "hello world" << endl;
    solve(0, NULL);
    return 0;    
}
 



