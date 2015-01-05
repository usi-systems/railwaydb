PROGRAM := $(shell basename "$(PWD)")

include ../Makefile.common

test:
	@echo `$(BINDIR)/$(PROGRAM) | grep -c "FAILURE" $<` "tests failed."


