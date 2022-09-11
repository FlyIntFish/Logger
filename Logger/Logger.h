#pragma once
#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <type_traits>
#include <time.h>

namespace Log{

	
	constexpr auto DEBUG =		"[debug] ";
	constexpr auto INFO =		"[info] ";
	constexpr auto TRACE =		"[trace] ";
	constexpr auto WARNING =	"[warning] ";
	constexpr auto ERROR =		"[error] ";
	constexpr auto CRITICAL =	"[critical] ";

	static unsigned constexpr MIN_BUFFER_SIZE = 500u;

class Logger
{
	static unsigned constexpr DEFAULT_BUFFER_SIZE = 8192u;
	static std::string defaultPathToLogs;

	std::string name;
	std::string pathToLogs;
	std::vector<char> buffer;
	std::ofstream file;
	std::ostream* output = &std::cout;
	size_t lastBufferSize = 0;
	unsigned flushWhenSizeWasReached = 0;
	bool writingToOutputEnabled = false;
	bool appendToFiles = false;

	[[nodiscard]] static std::string getTime();
	inline void clearBuffer()											{ buffer.clear(); }
	void checkBuffer();
	void writeToOfstream();
	void handleOutputWrite(const std::string & str) const;
	[[nodiscard]] std::string createLogBegin() const;

	struct LineBreaker
	{
		Logger& parent;
		bool passed = false;
		LineBreaker(Logger& logger) : parent(logger) {}
		LineBreaker(const LineBreaker&) = delete;
		LineBreaker(LineBreaker&& rval) noexcept;
		~LineBreaker();
		LineBreaker operator<<(const std::string& s);
		template <typename T>
		LineBreaker operator<<(const T& val);
	};
	
	friend LineBreaker;

public:
	explicit Logger(const std::string& name, bool appendToFiles = false, unsigned bufferSize = DEFAULT_BUFFER_SIZE);
	~Logger();

	
	[[nodiscard]] inline bool shouldWriteToFile() const					{ return buffer.size() >= flushWhenSizeWasReached; }

	inline void setBufferSize(unsigned newSize)							{ buffer.reserve(newSize); }
	[[nodiscard]] inline auto getBufferSize() const						{ return buffer.capacity(); }

	inline void setPathToLogs(std::string&& path)						{ pathToLogs = std::move(path); }
	inline void setPathToLogs(const std::string& path)					{ pathToLogs = path; }
	[[nodiscard]] inline const auto& getPathToLogs() const				{ return pathToLogs; }

	inline static void setDefaultPathToLogs(std::string&& path)			{ defaultPathToLogs = std::move(path); }
	inline static void setDefaultPathToLogs(const std::string& path)	{ defaultPathToLogs = path; }
	[[nodiscard]] inline static const auto& getDefaultPathToLogs()		{ return defaultPathToLogs; }

	inline void setWriteToOutput(bool flag)								{ writingToOutputEnabled = flag; }
	inline void setOutput(std::ostream& out)							{ output = &out; }
	[[nodiscard]] bool isWritingToOutputEnabled() const					{ return writingToOutputEnabled; }


	template <typename T>
	LineBreaker operator<<(const T& val);
	LineBreaker operator<<(const std::string& s);

	template <typename T>
	[[nodiscard]] static std::string asString(T&& value);

	template <typename... Args>
	void message(const std::string& formatter, Args&&... args);
	
	template <typename... Args>
	void debug(const std::string& formatter, Args&&... args);

	template <typename... Args>
	void info(const std::string& formatter, Args&&... args);

	template <typename... Args>
	void trace(const std::string& formatter, Args&&... args);

	template <typename... Args>
	void warning(const std::string& formatter, Args&&... args);

	template <typename... Args>
	void error(const std::string& formatter, Args&&... args);

	template <typename... Args>
	void critical(const std::string& formatter, Args&&... args);
};


template <typename... Args>
void Logger::message(const std::string& formatter, Args&&... args)
{
	checkBuffer();
	std::string extendedFormatter = "{} " + formatter + "\n";
	std::string constructedMessage = std::format(extendedFormatter, createLogBegin(), std::forward<Args>(args)...);
	buffer.insert(buffer.end(), constructedMessage.begin(), constructedMessage.end());
	handleOutputWrite(constructedMessage);
}


template <typename... Args>
void Logger::debug(const std::string& formatter, Args&&... args){
	message("{}" + formatter, DEBUG, std::forward<Args>(args)...);
}


template <typename... Args>
void Logger::info(const std::string& formatter, Args&&... args){
	message("{}" + formatter, INFO, std::forward<Args>(args)...);
}


template <typename... Args>
void Logger::trace(const std::string& formatter, Args&&... args){
	message("{}" + formatter, TRACE, std::forward<Args>(args)...);
}


template <typename... Args>
void Logger::warning(const std::string& formatter, Args&&... args){
	message("{}" + formatter, WARNING, std::forward<Args>(args)...);
}


template <typename... Args>
void Logger::error(const std::string& formatter, Args&&... args){
	message("{}" + formatter, ERROR, std::forward<Args>(args)...);
}


template <typename... Args>
void Logger::critical(const std::string& formatter, Args&&... args){
	message("{}" + formatter, CRITICAL, std::forward<Args>(args)...);
}

template <typename T>
Logger::LineBreaker Logger::operator<<(const T& val)
{
	return this->operator<<(asString(val));
}

template <typename T>
Logger::LineBreaker Logger::LineBreaker::operator<<(const T& val)
{
	return this->operator<<(asString(val));
}


template <typename T>
[[nodiscard]] static std::string Logger::asString(T&& value)
{
	if constexpr (std::is_same<std::decay<T>::type, const char*>::value || std::is_same<T, std::string>::value)
		return value;
	if constexpr (std::is_same<std::decay<T>::type, char>::value || std::is_same<std::decay<T>::type, wchar_t>::value)
		return { value };
	if constexpr (std::is_arithmetic<std::decay<T>::type>::value)
		return std::to_string(value);
	return "<UNSUPPORTED TYPE>";
}

}	//Log