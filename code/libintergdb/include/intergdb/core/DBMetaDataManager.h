#pragma once

#include <intergdb/common/SchemaStats.h>

namespace intergdb { namespace core
{     
    class DBMetaDataManager
    {        
    public:   
        DBMetaDataManager(std::string const & storageDir) : storageDir_(storageDir) {}
        void store(common::SchemaStats const & stats) 
        {
            std::cout << "DBMetaDataManager::store called" << std::endl;
        }

        common::SchemaStats load() 
        {
            common::SchemaStats stats;
            std::cout << "DBMetaDataManager::load called" << std::endl;
            return stats;
        }
    private:
        std::string storageDir_;
    };
} } /* namespace */


