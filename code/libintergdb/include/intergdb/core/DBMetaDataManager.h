#pragma once

#include <string>
#include <intergdb/common/SchemaStats.h>
#include <intergdb/core/NetworkByteBuffer.h>

namespace intergdb { namespace core
{     
    class DBMetaDataManager
    {        
    public:   
        DBMetaDataManager(std::string const & storageDir) : storageDir_(storageDir) {}
        void store(common::SchemaStats const & stats);
        void load(common::SchemaStats & stats);
    private:
        std::string storageDir_;
    };


    NetworkByteBuffer & operator << (NetworkByteBuffer & sbuf, common::SchemaStats const & stats);
    NetworkByteBuffer & operator >> (NetworkByteBuffer & sbuf, common::SchemaStats & stats);
    

} } /* namespace */


