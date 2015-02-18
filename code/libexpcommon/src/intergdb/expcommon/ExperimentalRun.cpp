#include <intergdb/expcommon/ExperimentalRun.h>

#include <intergdb/util/AutoTimer.h>

#include <boost/lexical_cast.hpp>

#include <cxxabi.h>
#include <typeinfo>
#include <iostream>

using namespace std;
using namespace intergdb::util;
using namespace intergdb::expcommon;

string ExperimentalRun::getFullyQualifiedClassName() 
{
  int status;
  char * realname = abi::__cxa_demangle(typeid(*this).name(), 0, 0, &status);
    string cname = realname;
  free(realname);
  return cname;
}

string ExperimentalRun::getClassName() 
{
  string fqname = getFullyQualifiedClassName();
  size_t loc = fqname.find_last_of("::");
  if (loc==string::npos)
    return fqname;
  return fqname.substr(loc+1);
}

void ExperimentalRun::run()
{
  cout << "Experiment '" + getClassName() + "' set up..." << endl;
  setUp();
  cout << "Experiment '" + getClassName() + "' started..." << endl;
  AutoTimer timer;
  timer.start();
  try {
    process();
  } catch (exception const & e) {
    cerr << "Exception caught, desc: " << e.what() << endl;
  } catch (...) {
    cerr << "Exception caught, desc: <unknown>" << endl;
  }
  timer.stop();
  cout << "Experiment '" + getClassName() + "' ended (took " + 
    boost::lexical_cast<string>(timer.getRealTimeInSeconds()) + " seconds)." << endl;
  cout << "Experiment '" + getClassName() + "' tear down..." << endl;
  tearDown();

}
