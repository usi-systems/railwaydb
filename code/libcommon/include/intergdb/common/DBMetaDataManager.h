#pragma once

#include <intergdb/common/SchemaStats.h>

namespace intergdb { namespace common
{     
    class DBMetaDataManager
    {        
    public:   
        DBMetaDataManager(std::string const & storageDir) : storageDir_(storageDir) {}
        void store(SchemaStats stats) 
        {
            std::cout << "DBMetaDataManager::store called" << std::endl;
        }

        SchemaStats load() 
        {
            SchemaStats stats;
            std::cout << "DBMetaDataManager::load called" << std::endl;
            return stats;
        }
    private:
        std::string storageDir_;
    };
} } /* namespace */


