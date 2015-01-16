THISDIR := $(realpath .)

LIBRARY = liboptimizer.so

CPPFLAGS =

INCLUDES = \
 -I $(THISDIR)/../libcommon/include \
 -I $(THISDIR)/../libintergdb/include \
 -I $(GUROBI_HOME)/include

LDFLAGS = \
 -L $(GUROBI_HOME)/lib \
 -L $(THISDIR)/../libcommon/bin \
 -lcommon \
 -lgurobi56 \
 -lpthread \
 -lm 

include ../Makefile.lib