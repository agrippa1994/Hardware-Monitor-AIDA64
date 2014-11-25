#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <vector>

namespace aida64
{
	struct Data
	{
		std::string category;
		std::string id;
		std::string label;
		std::string value;
	};

	class API
	{
		std::vector<Data> data_;
		HANDLE shm_ = INVALID_HANDLE_VALUE;

		std::vector<Data> readData();

	public:
		API(const std::string& shmName = "AIDA64_SensorValues");
		~API();

		bool refreshData();
		const std::vector<Data>& data() const;
	};
}