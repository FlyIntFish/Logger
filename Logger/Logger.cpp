#include "Logger.h"
namespace Log
{

std::string Logger::defaultPathToLogs = std::filesystem::current_path().string();

Logger::Logger(const std::string& name_, bool immediateWrite_, unsigned bufferSize) :
	name(name_),
	immediateWrite(immediateWrite_),
	pathToLogs(defaultPathToLogs)
{
	buffer.reserve(bufferSize);
}

std::string Logger::getTime()
{
	auto time_tValue = time(nullptr);
	tm currentTime;
	localtime_s(&currentTime, &time_tValue);
	return std::format(
		"[{}/{}/{} {}:{}:{}] ",
		currentTime.tm_mday,
		currentTime.tm_mon + 1,
		currentTime.tm_year + 1900,
		currentTime.tm_hour,
		currentTime.tm_min,
		currentTime.tm_sec
	);
}


} // Log