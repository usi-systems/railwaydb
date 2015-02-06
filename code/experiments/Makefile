THISDIR := $(realpath .)

EXECUTABLE = experiments

CPPFLAGS = 

INCLUDES = \
	-isystem /opt/local/libcpp-software/include \
	-I $(THISDIR)/../ext/include \
    -I $(THISDIR)/../ext/src/leveldb/include \
	-I $(THISDIR)/../libcommon/include \
	-I $(THISDIR)/../liboptimizer/include \
	-I $(THISDIR)/../libintergdb/include \

LDFLAGS = \
	-L /opt/local/libcpp-software/lib \
	-L $(THISDIR)/../ext/lib \
    -L $(THISDIR)/../ext/src/leveldb \
	-L $(THISDIR)/../libcommon/bin  \
	-L $(THISDIR)/../liboptimizer/bin \
	-L $(THISDIR)/../libintergdb/bin \
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
    -lcommon \
    -loptimizer \
    -lintergdb 

include ../Makefile.exe
