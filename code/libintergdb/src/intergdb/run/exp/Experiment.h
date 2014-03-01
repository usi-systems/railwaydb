#ifndef INTERGDB_RUN_EXP_EXPERIMENT_H
#define INTERGDB_RUN_EXP_EXPERIMENT_H

#define BOOST_NO_CXX11_RVALUE_REFERENCES

#include <string>
#include <fstream>
#include <vector>
#include <tr1/unordered_map>
#include <boost/variant.hpp>

namespace intergdb { namespace run { namespace exp {

class Experiment 
{
public:
	typedef boost::variant<size_t, double, std::string> any_value; 
public:
	Experiment(std::string const & name, 
			   std::string const & dirName = "expData");
	void setDescription(std::string const & desc);
	void setKeepValues(bool keep);
	void addFieldName(std::string const & field);
	void open();
	void addNewRecord();
	void addNewFieldValue(int index, any_value const & value);
	void addNewFieldValue(std::string const & name, any_value const & value);
	void appendNewFieldValue(any_value const & value);
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
	std::tr1::unordered_map<std::string, size_t> fieldNameIndices_;
	std::vector<std::vector<any_value> > values_;
};

}}}

#endif /* INTERGDB_RUN_EXP_EXPERIMENT_H */
