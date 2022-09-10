#pragma once
#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <filesystem>
#include <type_traits>
#include <time.h>

namespace Log{

	constexpr auto debug =		"[debug] ";
	constexpr auto info =		"[info] ";
	constexpr auto trace =		"[trace] ";
	constexpr auto warning =	"[warning] ";
	constexpr auto error =		"[error] ";
	constexpr auto critical =	"[critical] ";

class Logger
{
public:
	static unsigned constexpr defaultBufferSize = 8192u;
	static std::string defaultPathToLogs;

	std::string name;
	std::string pathToLogs;
	std::vector<char> buffer;
	bool immediateWrite = false;
	
public:
	explicit Logger(const std::string& name, bool immediateWrite = false, unsigned bufferSize = defaultBufferSize);

	inline void setBufferSize(unsigned newSize)							{ buffer.reserve(newSize); }
	[[nodiscard]] inline auto getBufferSize() const						{ return buffer.capacity(); }
	inline void clearBuffer()											{ buffer.clear(); }

	[[nodiscard]] inline const auto& getPathToLogs() const				{ return pathToLogs; }
	inline void setPathToLogs(const std::string& path)					{ pathToLogs = path; }
	inline void setPathToLogs(std::string&& path)						{ pathToLogs = std::move(path); }

	[[nodiscard]] inline static const auto& getDefaultPathToLogs()		{ return defaultPathToLogs; }
	inline static void setDefaultPathToLogs(const std::string& path)	{ defaultPathToLogs = path; }
	inline static void setDefaultPathToLogs(std::string&& path)			{ defaultPathToLogs = std::move(path); }

	template <typename T>
	static std::string asString(T&& value)
	{
		if constexpr (std::is_same<std::decay<T>::type, const char*>::value || std::is_same<T, std::string>::value)
			return value;
		if constexpr (std::is_same<T, char>::value || std::is_same<T, wchar_t>::value)
			return { value };
		if constexpr (std::is_arithmetic<T>::value)
			return std::to_string(value);
		return "<UNSUPPORTED TYPE>";
	}
	
	struct LineBreaker
	{
		std::vector<char>* buffer;
		bool passed = false;
		LineBreaker(std::vector<char>* v) : buffer(v) {}
		LineBreaker(const LineBreaker&) = delete;
		LineBreaker(LineBreaker&& rval) noexcept
		{
			buffer = rval.buffer;
			rval.passed = true;
		}
		~LineBreaker() {
			if (!passed) buffer->push_back('\n');
		}
		LineBreaker operator<<(const std::string& s) {
			buffer->insert(buffer->end(), s.begin(), s.end());
			return std::move(*this);
		}
	};
	LineBreaker operator<<(const std::string& s) {
		return LineBreaker(&buffer) << getTime() << s;
	}

	[[nodiscard]] static std::string getTime();

	inline void writeToOfstream(std::ostream& out) const { out.write(&buffer[0], buffer.size()); }

	template <typename... Args>
		void message(Args&&... args);

	template <typename T, typename... Args>
		void _message(T&& obj, Args&&... args);

	void _message() { buffer.push_back('\n'); }
	
};

template <typename... Args>
void Logger::message(Args&&... args)
{
	auto string = std::move(getTime());
	buffer.insert(buffer.end(), string.begin(), string.end());
	_message(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
void Logger::_message(T&& obj, Args&&... args)
{
	auto string = asString(std::forward<T>(obj));
	buffer.insert(buffer.end(), string.begin(), string.end());
	_message(std::forward<Args>(args)...);
}



}	//Log

