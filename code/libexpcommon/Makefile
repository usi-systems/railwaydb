THISDIR := $(realpath .)

LIBRARY = libexpcommon.so

CPPFLAGS = 

INCLUDES = \
	-isystem /opt/local/libcpp-software/include 

LDFLAGS = \
	-L /opt/local/libcpp-software/lib \
	-lboost_filesystem-mt \
	-lboost_iostreams-mt \
	-lboost_chrono-mt \
	-lboost_date_time-mt \
	-lboost_system-mt \
	-lboost_thread-mt 

include ../Makefile.lib
