#include <intergdb/run/Run.h>

#include <intergdb/run/AutoTimer.h>

#include <boost/lexical_cast.hpp>

#include <cxxabi.h>
#include <typeinfo>
#include <iostream>

using namespace std;
using namespace intergdb::run;

string Run::getClassName() 
{
	int status;
	char * realname = abi::__cxa_demangle(typeid(*this).name(), 0, 0, &status);
    string cname = realname;
	free(realname);
	return cname;
}

void Run::run()
{
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
}
