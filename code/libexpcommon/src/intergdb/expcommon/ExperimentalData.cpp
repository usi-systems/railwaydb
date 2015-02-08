#include <intergdb/expcommon/ExperimentalData.h>

#include <boost/filesystem.hpp>
#include <stdexcept>

using namespace std;
using namespace intergdb;
using namespace intergdb::expcommon;

ExperimentalData::ExperimentalData(string const & expName, 
  string const & dirName /*="."*/)
  : keepValues_(false),
    currentFieldIndex_(0),
    name_ (expName),
    dirName_ (dirName),
    ext_(".dat"),
    fileName_(name_+ext_)
{} 

void ExperimentalData::setDescription(std::string const & desc)
{
  description_ = desc;
}

void ExperimentalData::setKeepValues(bool keep) 
{
  keepValues_ = keep;
}

void ExperimentalData::addField(string const & field)
{
  fieldNameIndices_.insert(make_pair(field, fieldNames_.size()));
    fieldNames_.push_back(field);
}

void ExperimentalData::open()
{
  using namespace boost::filesystem;
  path dir(dirName_);
  create_directory(dir);
  file_.open((dirName_+"/"+fileName_).c_str(), ios::out);
  if (!description_.empty())
    file_ << "# " << description_ << endl;
  file_ << "# ";
  for (size_t i=0, iu=fieldNames_.size(); i<iu; ++i)
    file_ << fieldNames_[i] << "\t";
  file_.flush();
}

void ExperimentalData::addRecord()
{
  file_ << endl;
  if (keepValues_) {
    currentFieldIndex_ = 0;
    values_.push_back(vector<any_value>(fieldNames_.size(), any_value()));
  }
}

void ExperimentalData::setFieldValue(int fieldIndex, any_value const & value)
{
  file_ << value << "\t";
  file_.flush();
  if (keepValues_) 
    values_.back()[static_cast<size_t>(fieldIndex)] = value;
}

void ExperimentalData::setFieldValue(std::string const & fieldName, any_value const & value)
{
  if (fieldNameIndices_.count(fieldName)==0) 
    throw runtime_error("field '"+fieldName+"' not found!");
  size_t index = fieldNameIndices_[fieldName];
  setFieldValue(index, value);
}

void ExperimentalData::appendFieldValue(any_value const & value)
{
  setFieldValue(currentFieldIndex_++, value);
}

void ExperimentalData::close()
{
  file_.close();
}

vector<vector<ExperimentalData::any_value> > const & ExperimentalData::getValues() const
{
  return values_;
}


