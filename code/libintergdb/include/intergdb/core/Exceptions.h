#pragma once

#include <intergdb/core/Types.h>

#include <boost/lexical_cast.hpp>
#include <stdexcept>

namespace intergdb { namespace core
{
    class vertex_already_exists_exception : public std::runtime_error
    {
    public:
        vertex_already_exists_exception(VertexId id)
            : std::runtime_error("vertex " +
                boost::lexical_cast<std::string>(id) + " already exists")
        {}
    };

    class vertex_not_found_exception : public std::runtime_error
    {
    public:
        vertex_not_found_exception(VertexId id)
            : std::runtime_error("vertex " +
                    boost::lexical_cast<std::string>(id) + " not found")
        {}
    };

    class block_already_exists_exception : public std::runtime_error
    {
    public:
        block_already_exists_exception(BlockId id)
            : std::runtime_error("block " +
                boost::lexical_cast<std::string>(id) + " already exists")
        {}
    };

    class block_not_found_exception : public std::runtime_error
    {
    public:
        block_not_found_exception(BlockId id)
          : std::runtime_error("block " +
                boost::lexical_cast<std::string>(id) + " not found")
        {}
    };

    class attribute_not_found_exception : public std::runtime_error
    {
    public:
        attribute_not_found_exception(std::string const & description)
            : std::runtime_error("attribute not found: " + description)
        {}
    };

    class time_sliced_partition_not_found_exception : public std::runtime_error
    {
    public:
        time_sliced_partition_not_found_exception(Timestamp time)
            : std::runtime_error(
                "time sliced partition that contains timestamp " +
                    boost::lexical_cast<std::string>(time) + " not found")
        {}
    };

    class time_sliced_partition_replacement_exception : public std::runtime_error
    {
    public:
        time_sliced_partition_replacement_exception(
            std::string const & description)
            : std::runtime_error(
                "error in replacing time sliced partition(s): " + description)
        {}
    };


} } /* namespace */
