THISDIR := $(realpath .)

LIBRARY = liboptimizer.so

CPPFLAGS =

INCLUDES = \
 -I $(THISDIR)/../libcommon/include \
 -I $(THISDIR)/../libintergdb/include \
 -I $(THISDIR)/../ext/include \
 -I $(GUROBI_HOME)/include

LDFLAGS = \
 -L $(GUROBI_HOME)/lib \
 -L $(THISDIR)/../libcommon/bin \
 -lcommon \
 -lgurobi60 \
 -lpthread \
 -lm 

include ../Makefile.lib