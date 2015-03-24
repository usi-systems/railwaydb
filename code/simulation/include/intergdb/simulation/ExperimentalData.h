#pragma once

#include <boost/variant.hpp>

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace intergdb { namespace simulation {

class ExperimentalData
{
public:
    typedef boost::variant<size_t, double, std::string> any_value;

    ExperimentalData(std::string const & name,
                     std::string const & dirName = "expData");

    void setDescription(std::string const & desc);

    void setKeepValues(bool keep);

    void addField(std::string const & field);

    void open();

    void addRecord();

    void setFieldValue(int index, any_value const & value);

    void setFieldValue(std::string const & name, any_value const & value);

    void appendFieldValue(any_value const & value);

    void close();

    std::vector<std::vector<any_value> > const & getValues() const;

private:

    bool keepValues_;
    size_t currentFieldIndex_;
    std::string description_;
    std::string name_;
    std::string dirName_;
    std::string const ext_;
    std::string fileName_;
    std::fstream file_;
    std::vector<std::string> fieldNames_;
    std::unordered_map<std::string, size_t> fieldNameIndices_;
    std::vector<std::vector<any_value> > values_;
};
}}

