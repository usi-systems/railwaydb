#ifndef __DRIVER_H__
#define __DRIVER_H__ 1

#include <string>
#include "Scanner.h"
#include "Parser.tab.hh"

namespace intergdb {
    namespace lang {
   
        class Driver {    
        public:
        Driver() : 
            parser( nullptr ),
            scanner( nullptr ){};
            virtual ~Driver();

            void parse( const char *filename );  
            void printVertex();
            void printEdge();

        private:
            Parser *parser;
            Scanner *scanner;
        };

    } /* end namespace lang */
} /* end namespace intergdb */

#endif /* END __DRIVER_H__ */
