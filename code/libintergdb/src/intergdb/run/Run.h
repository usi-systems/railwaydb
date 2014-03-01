#ifndef INTERGDB_RUN_RUN_H
#define INTERGDB_RUN_RUN_H

#define BOOST_NO_CXX11_RVALUE_REFERENCES

#include <string>

namespace intergdb { namespace run
{
    class Run {
    public:
        virtual ~Run() {}
        virtual void process() = 0;
        void run();
    protected:
        std::string getClassName();
    };

} } /* namespace */

#endif /* INTERGDB_RUN_RUN_H */
