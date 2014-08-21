TESTS   = OptimizerTest

all:
	find . -name "*Test" -type d -exec sh -c "cd {}; pwd; make" \;

test: $(TESTS)
	@- $(foreach TEST,$(TESTS), \
		find . -name $(TEST) -type d -exec sh -c "cd {}; pwd; make test" \; \
    )

clean:
	find . -name "*Test" -type d -exec sh -c "cd {}; make clean" \;
