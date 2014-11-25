#include "aida64.h"
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

std::vector<aida64::Data> aida64::API::readData()
{
	if (shm_ == INVALID_HANDLE_VALUE)
		throw std::exception("Invalid handle value");

	auto pStr = MapViewOfFile(shm_, FILE_MAP_READ, 0, 0, 0);
	if (pStr == nullptr)
		throw std::exception("Can't read file");

	std::stringstream ss(std::string((const char *)pStr));
	std::vector<Data> data;

	
	boost::property_tree::ptree pt;
	boost::property_tree::read_xml(ss, pt);

	for (const auto& i : pt)
	{
		if (i.second.size() != 3)
			throw std::exception("Wrong format");

		int idx = 0;

		Data currentData;
		currentData.category = i.first;

		for (const auto & u : i.second)
		{
			std::string value = u.second.get_value<std::string>();

			if (idx == 0)	currentData.id = value;
			else if (idx == 1)	currentData.label = value;
			else if (idx == 2)	currentData.value = value;
			else throw std::exception("Error while decomposing data");
			idx++;
		}

		data.push_back(currentData);
	}

	return data;
}

aida64::API::API(const std::string& shmName /*= "AIDA64_SensorValues"*/)
{
	if ((shm_ = OpenFileMappingA(FILE_MAP_READ, FALSE, shmName.c_str())) == INVALID_HANDLE_VALUE)
		throw std::exception("Shared Memory cannot be opened");
}

aida64::API::~API()
{
	if (shm_ != INVALID_HANDLE_VALUE)
		CloseHandle(shm_);
}

bool aida64::API::refreshData()
{
	data_ = readData();
	if (data_.empty())
		return false;

	return true;
}

const std::vector<aida64::Data>& aida64::API::data() const
{
	return data_;
}
