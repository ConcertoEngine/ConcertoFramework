//
// Created by arthur on 25/05/22.
//

#ifndef CONCERTO_CORE_LOGGER_HPP
#define CONCERTO_CORE_LOGGER_HPP

#include <format>
#include <iostream>
#include <source_location>

#include "Concerto/Core/Types/Types.hpp"

namespace cct
{
	namespace Terminal::Color
	{
		static constexpr auto DEFAULT = "\x1B[0m";
		static constexpr auto RED = "\x1B[31m";
		static constexpr auto GREEN = "\x1B[32m";
		static constexpr auto YELLOW = "\x1B[33m";
		static constexpr auto BLUE = "\x1B[34m";
		static constexpr auto MAGENTA = "\x1B[35m";
		static constexpr auto CYAN = "\x1B[36m";
	}// namespace Terminal::Color
	class CCT_CORE_PUBLIC_API Logger
	{
	 public:
		template<typename... T>
		struct Debug
		{
			explicit Debug(const std::format_string<T...> fmt, T&&... args, std::source_location loc = std::source_location::current())
			{
				Log(std::vformat(fmt.get(), std::make_format_args(args...)), LogLevel::Debug, loc);
#ifdef CCT_PLATFORM_WINDOWS
				DebugString(std::vformat(fmt.get(), std::make_format_args(args...)));
#endif
			}
		};
		enum class LogLevel
		{
			Debug,
			Info,
			Warning,
			Error
		};

		/**
	    * @brief Log a message with the DEBUG level = INFO
	    * @param fmt The format of the message to Log
	    * @param args The arguments of the message to Log
	    */
		template<typename... Types>
		static void Info(const std::format_string<Types...> fmt, Types&&... args)
		{
			Log(std::vformat(fmt.get(), std::make_format_args(args...)), LogLevel::Info);
		}

		/**
	    * @brief Log a message with the DEBUG level = DEBUG
	    * @param fmt The format of the message to Log
	    * @param args The arguments of the message to Log
	    * @attention see https://cor3ntin.github.io/posts/variadic/
	    */
		template<typename... Types>
		Debug(std::format_string<Types...>, Types&&...) -> Debug<Types...>;

		/**
	    * @brief Log a message with the DEBUG level = WARNING
	    * @param fmt The format of the message to Log
	    * @param args The arguments of the message to Log
	    */
		template<typename... Types>
		static void Warning(const std::format_string<Types...> fmt, Types&&... args)
		{
			Log(std::vformat(fmt.get(), std::make_format_args(args...)), LogLevel::Warning);
		}

		/**
	    * @brief Log a message with the DEBUG level = ERROR
		* @param fmt The format of the message to Log
	    * @param args The arguments of the message to Log
	    */
		template<typename... Types>
		static void Error(const std::format_string<Types...> fmt, Types&&... args)
		{
			Log(std::vformat(fmt.get(), std::make_format_args(args...)), LogLevel::Error);
		}

		/**
	    * @brief Log a message
	    * @param level The level of the message
	    * @param message The message to Log
	    * @param location The location of the message
	    */
		template<typename T>
		static void Log(const T& message, LogLevel level, const std::source_location& location = std::source_location::current())
		{
			switch (level)
			{
			case LogLevel::Debug:
				std::cerr << Terminal::Color::CYAN << location.file_name() << ": " << location.line() << ": " << message << Terminal::Color::DEFAULT << '\n';
				break;
			case LogLevel::Info:
				std::cout << Terminal::Color::GREEN << message << Terminal::Color::DEFAULT << '\n';
				break;
			case LogLevel::Warning:
				std::cout << Terminal::Color::YELLOW << message << Terminal::Color::DEFAULT << '\n';
				break;
			case LogLevel::Error:
				std::cerr << Terminal::Color::RED << message << Terminal::Color::DEFAULT << '\n';
				break;
			}
		}

		static void DebugString(std::string_view string);
	};
}// namespace cct
#endif//CONCERTO_CORE_LOGGER_HPP