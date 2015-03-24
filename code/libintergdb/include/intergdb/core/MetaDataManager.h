#pragma once

#include <intergdb/common/SchemaStats.h>
#include <intergdb/core/NetworkByteBuffer.h>
#include <intergdb/core/Types.h>

#include <string>

namespace intergdb { namespace core
{
    class MetaDataManager
    {
    public:
        MetaDataManager(std::string const & storageDir);

        void store();

        void load();

        common::SchemaStats & getSchemaStats()
        {
            return schemaStats_;
        }

        BlockId & getNextBlockId()
        {
            return nextBlockId_;
        }

    private:
        std::string storageDir_;
        BlockId nextBlockId_;
        common::SchemaStats schemaStats_;
    };

    NetworkByteBuffer & operator << (NetworkByteBuffer & sbuf,
                                     common::SchemaStats const & stats);
    NetworkByteBuffer & operator >> (NetworkByteBuffer & sbuf,
                                     common::SchemaStats & stats);
} } /* namespace */


