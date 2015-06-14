THISDIR := $(realpath .)

EXECUTABLE = run_all_tests

CPPFLAGS =

INCLUDES = \
	-isystem /opt/local/libcpp-software/include \
 	-I $(THISDIR)/../ext/include \
 	-I $(THISDIR)/../ext/src/leveldb/include \
 	-I $(THISDIR)/../libcommon/include \
 	-I $(THISDIR)/../libintergdb/include \
 	-I $(THISDIR)/../liboptimizer/include

LDFLAGS = \
	-L $(THISDIR)/../ext/lib \
	-L $(THISDIR)/../ext/src/leveldb \
	-L $(THISDIR)/../libcommon/bin \
	-L $(THISDIR)/../libintergdb/bin \
	-L $(THISDIR)/../liboptimizer/bin \
	-L /opt/local/libcpp-software/lib \
	-lboost_chrono-mt \
	-lboost_context-mt \
	-lboost_coroutine-mt \
	-lboost_date_time-mt \
	-lboost_filesystem-mt \
	-lboost_iostreams-mt \
	-lboost_system-mt \
	-lboost_thread-mt \
	-lcommon \
	-lgtest \
	-lintergdb \
	-lleveldb \
	-loptimizer \
	-lspatialindex

include ../Makefile.exe