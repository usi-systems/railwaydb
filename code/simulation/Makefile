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

OBJDIR  = $(realpath .)/obj
BINDIR  = $(realpath .)/bin
SRCDIR  = $(realpath src)
INCDIR  = $(realpath include) 

EXECUTABLE   = simulation

CPPFLAGS  = 
INCLUDES  = -isystem /opt/local/libcpp-software/include \
 -I $(realpath $(THISDIR)../libcommon/include) \
 -I $(realpath $(THISDIR)../liboptimizer/include) \
 -I $(realpath $(THISDIR)../libintergdb/include) 
LDFLAGS   = -L /opt/local/libcpp-software/lib -L ../libcommon/bin  -L ../liboptimizer/bin \
 -lboost_filesystem-mt -lboost_iostreams-mt -lboost_chrono-mt \
 -lboost_date_time-mt -lboost_system-mt -lboost_thread-mt -lcommon -loptimizer

## Implicit Section: change the following only when necessary.
CXX      = clang++ 
CXXFLAGS = -g -O2 -fPIC -Wall -std=c++11 -stdlib=libc++  
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

all: $(BINDIR)/$(EXECUTABLE)

$(OBJDIR)/%.d: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@/bin/echo -n $(dir $@) > $@
	@$(DEPEND) $< >> $@ || rm $@

objs: $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(COMPILE) -c $< -o $@

$(BINDIR)/$(EXECUTABLE): $(OBJS)
	@mkdir -p $(dir $@)
	$(LINK) $(OBJS) -o "$@"

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



