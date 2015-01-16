THISDIR := $(realpath .)

EXECUTABLE = simulation

CPPFLAGS = 

INCLUDES = \
	-isystem /opt/local/libcpp-software/include \
	-I $(THISDIR)/../libcommon/include \
	-I $(THISDIR)/../liboptimizer/include \
	-I $(THISDIR)/../libintergdb/include \

LDFLAGS = \
	-L /opt/local/libcpp-software/lib \
	-L $(THISDIR)/../libcommon/bin  \
	-L $(THISDIR)/../liboptimizer/bin \
	-lboost_filesystem-mt \
	-lboost_iostreams-mt \
	-lboost_chrono-mt \
	-lboost_date_time-mt \
	-lboost_system-mt \
	-lboost_thread-mt \
	-lcommon \
	-loptimizer

include ../Makefile.exe