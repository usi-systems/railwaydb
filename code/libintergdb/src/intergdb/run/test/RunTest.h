#ifndef INTERGDB_RUN_TEST_RUNTEST_H
#define INTERGDB_RUN_TEST_RUNTEST_H

#include <intergdb/run/Run.h>

namespace intergdb { namespace run { namespace test {

    class RunGeneratorTest : public Run
    {
    public:
        virtual void process();
    };


    class RunInteractionGraphTest : public Run
    {
    public:
        virtual void process();
    };

}}}

#endif /* INTERGDB_RUN_TEST_RUNTEST_H */
