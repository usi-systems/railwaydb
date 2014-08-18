# Make Target:
# ------------
# The Makefile provides the following targets to make:
#   $ make           compile and link
#   $ make objs      compile only (no linking)
#   $ make docs     build documentation (requires doxygen)
#   $ make clean     clean objects and the executable file
#   $ make help      get the usage of the makefile

## Customizable Section: adapt those variables to suit your program.

THISDIR := $(dir $(lastword $(MAKEFILE_LIST)))

OBJDIR  = obj
BINDIR  = bin
SRCDIR  = src
INCDIR  = include

LIBRARY   = liboptimizer.so

CPPFLAGS  = 
INCLUDES  = \
 -I ../libcommon/include \
 -I $(GUROBI_HOME)/include

LDFLAGS   =  -L $(GUROBI_HOME)/lib \
 -L ../libcommon/bin \
 -lcommon \
 -lgurobi56 \
 -lpthread \
 -lm 


## Implicit Section: change the following only when necessary.
CXX      = clang++ 
CXXFLAGS = -g -O2 -fPIC -Wall -stdlib=libc++ -std=c++11 
SRCDIRS  = $(shell find $(SRCDIR) -type d)
INCDIRS  = $(shell find $(INCDIR) -type d)
INCFLAGS = $(addprefix -I ,$(SRCDIRS)) $(addprefix -I ,$(INCDIRS)) $(INCLUDES)

## Stable Section: usually no need to be changed. 
SHELL   = /bin/sh
SOURCES = $(wildcard $(addsuffix /*.cpp,$(SRCDIRS)))
OBJS    = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SOURCES:.cpp=.o))
DEPS    = $(OBJS:.o=.d)

## Define some useful variables.
CDP	= clang++
DEP_OPT = -MM -MP
DEPEND  = $(CDP) $(CPPFLAGS) $(CXXFLAGS) $(INCFLAGS) $(DEP_OPT)
COMPILE = $(CXX) $(CPPFLAGS) $(CXXFLAGS) $(INCFLAGS) 
LINK    = $(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS)

.PHONY: all objs clean distclean help show

# Delete the default suffixes
.SUFFIXES:

all: $(BINDIR)/$(LIBRARY)

$(OBJDIR)/%.d: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@/bin/echo -n $(dir $@) > $@
	@$(DEPEND) $< >> $@ || rm $@

objs: $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(COMPILE) -c $< -o $@

$(BINDIR)/$(LIBRARY): $(OBJS)
	@mkdir -p $(dir $@)
	$(LINK) $(OBJS) -shared -o "$(CURDIR)/$@"

docs:
	doxygen doc/doxy.conf

ifneq ($(DEPS),)
  sinclude $(DEPS)
endif

clean:
	$(RM) -r $(OBJDIR) $(BINDIR)
	$(RM) -r docs/html

help:
	@echo 'Usage: make [TARGET]'
	@echo 'TARGETS:'
	@echo '  all       (=make) compile and link.'
	@echo '  objs      compile only (no linking).'
	@echo '  tags      create tags for Emacs editor.'
	@echo '  clean     clean objects and the executable file.'
	@echo '  show      show variables (for debug use only).'
	@echo '  help      print this message.'

show:
	@echo 'LIBRARY     :' $(LIBRARY)
	@echo 'SRCDIRS     :' $(SRCDIRS)
	@echo 'INCDIRS     :' $(INCDIRS)
	@echo 'SOURCES     :' $(SOURCES)
	@echo 'OBJS        :' $(OBJS)
	@echo 'DEPS        :' $(DEPS)
	@echo 'DEPEND      :' $(DEPEND)
	@echo 'COMPILE     :' $(COMPILE)
	@echo 'link        :' $(LINK)
