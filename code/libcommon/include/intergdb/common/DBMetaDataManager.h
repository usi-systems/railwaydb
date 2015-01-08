#pragma once

#include <intergdb/common/SchemaStats.h>

namespace intergdb { namespace common
{     
    class DBMetaDataManager
    {        
    public:
        DBMetaDataManager(/* Conf const & conf */) /*: conf_(conf)*/ {}

        void store(SchemaStats stats) 
        {

        }

        SchemaStats load() 
        {
            SchemaStats stats;
            return stats;
        }
    private:
        // Conf conf_;        
    };
} } /* namespace */


