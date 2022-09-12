#include "Logger.h"
namespace Log
{

std::string Logger::defaultPathToLogs = std::filesystem::current_path().string();

Logger::Logger(const std::string& name_, bool appendToFiles_, unsigned bufferSize, const std::string& pathToLogsDir) :
	name(name_),
	pathToLogs(pathToLogsDir),
	appendToFiles(appendToFiles_)
{
	if (bufferSize < MIN_BUFFER_SIZE)
	{
		std::cerr << "Buffer size for logger " << name << " cannot be lower than " << MIN_BUFFER_SIZE << "b\n";
		bufferSize = MIN_BUFFER_SIZE;
	}
	flushWhenSizeWasReached = static_cast<unsigned>(bufferSize - 200);
	buffer.reserve(bufferSize);
	auto filenameWithPath = pathToLogs + "/" + name + ".txt";
	if(appendToFiles)
		file.open(filenameWithPath, std::ios_base::app);
	else
		file.open(filenameWithPath);
	if (!file.good())
		std::cout << "! Logger \"" << name << "\" cannot open file " << filenameWithPath << "\n";
}

Logger::~Logger()
{
	writeToOfstream();
	if(file.is_open())
		file.close();
}

void Logger::writeToOfstream(){
	file.write(&buffer[0], buffer.size());
}

std::string Logger::getTime()
{
	auto time_tValue = time(nullptr);
	static tm currentTime;
	localtime_s(&currentTime, &time_tValue);
	return std::format(
		"[{}/{:02}/{:02} {:02}:{:02}:{:02}]",
		currentTime.tm_year + 1900,
		currentTime.tm_mon + 1,
		currentTime.tm_mday,
		currentTime.tm_hour,
		currentTime.tm_min,
		currentTime.tm_sec
	);
}

void Logger::checkBuffer()
{
	if (shouldWriteToFile())
	{
		writeToOfstream();
		clearBuffer();
	}
}

std::string Logger::createLogBegin() const
{
	return std::string{ std::format("[{}] {}", name, getTime()) };
}

void Logger::handleOutputWrite(const std::string& str) const
{
	if (isWritingToOutputEnabled())
		*output << str;
}

void Logger::clearBuffer() 
{ 
	buffer.clear();
}

Logger::LineBreaker Logger::operator<<(const std::string& s) {
	checkBuffer();
	if (isWritingToOutputEnabled())
		lastBufferSize = buffer.size();
	return LineBreaker(*this) << createLogBegin() << " " << s;
}

Logger::LineBreaker::LineBreaker(LineBreaker&& rval) noexcept : parent(rval.parent)
{
	rval.passed = true;
}

Logger::LineBreaker::~LineBreaker() {
	if (!passed)
	{
		parent.buffer.push_back('\n');
		if (parent.isWritingToOutputEnabled())
		{
			size_t messageLen = parent.buffer.size() - parent.lastBufferSize;
			parent.output->write( &parent.buffer[parent.lastBufferSize], messageLen);
		}
	}
}

Logger::LineBreaker Logger::LineBreaker::operator<<(const std::string& s)
{
	parent.buffer.insert(parent.buffer.end(), s.begin(), s.end());
	return std::move(*this);
}

} // Log
