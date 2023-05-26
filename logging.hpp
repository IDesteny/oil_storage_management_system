#ifndef __LOGGING_HPP__
#define __LOGGING_HPP__

#include <iostream>
#include <experimental/source_location>

#include "dye.hpp"

static std::mutex logging_mutex;

class logging
{
	[[nodiscard]] static std::string get_current_time()
	{
		auto now = std::time(nullptr);
		auto time = std::stringstream();
		time << std::put_time(localtime(&now), "%T");
		return time.str();
	}

public:
	template <typename T>
	static void errlog(
		T &&message,
		std::experimental::source_location location = std::experimental::source_location::current())
	{
		auto terminal_dye = dye();
		auto errinfo = std::stringstream();
		
		errinfo << "file: " <<
			location.file_name() << '(' <<
			location.line() << ':' <<
			location.column() << ") `" <<
			location.function_name() << "` " <<
			std::forward<T>(message);
		
		log(terminal_dye.colorant("ERROR", dye::code::red), errinfo.str());
	}

	template <typename T>
	static void warnlog(T &&message)
	{
		log(dye().colorant("WARN", dye::code::yellow), std::forward<T>(message));
	}
	
	template <typename T>
	static void inflog(T &&message)
	{
		log("INFO", std::forward<T>(message));
	}

	template <typename T>
	static void log(std::string_view type, T &&message)
	{
		std::lock_guard guard(logging_mutex);

		std::clog << '[' << type << "] [" << get_current_time() << "] " << std::forward<T>(message) << std::endl;
	}
};

#endif // !__LOGGING_HPP__
