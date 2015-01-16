THISDIR := $(realpath .)

EXECUTABLE = simulation

CPPFLAGS = 

EXECUTABLE = run_all_tests

CPPFLAGS =

INCLUDES = \
	-isystem /opt/local/libcpp-software/include \
 	-I $(THISDIR)/../libintergdb/include \
 	-I $(THISDIR)/../ext/include \
 	-I $(THISDIR)/../ext/src/leveldb/include \
 	-I $(THISDIR)/../libcommon/include \
 	-I $(THISDIR)/../liboptimizer/include

LDFLAGS = \
	-L /opt/local/libcpp-software/lib \
	-L $(THISDIR)/libintergdb/bin \
	-L $(THISDIR)/ext/lib \
	-L $(THISDIR)/ext/src/leveldb \
	-L $(THISDIR)/libcommon/bin \
	-L $(THISDIR)/liboptimizer/bin \
	-lboost_coroutine-mt \
	-lboost_context-mt \
	-lboost_filesystem-mt \
	-lboost_iostreams-mt \
	-lboost_chrono-mt \
	-lboost_date_time-mt \
	-lboost_system-mt \
	-lboost_thread-mt \
	-lleveldb \
	-lspatialindex \
	-lintergdb \
	-lcommon \
	-loptimizer \
	-lgtest 

include ../Makefile.exe