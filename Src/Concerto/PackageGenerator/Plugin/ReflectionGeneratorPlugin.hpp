//
// Created by arthur
//

#ifndef CONCERTO_PKGGENERATOR_REFLECTIONGENERATORPLUGIN_HPP
#define CONCERTO_PKGGENERATOR_REFLECTIONGENERATORPLUGIN_HPP

#include <format>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "Concerto/PackageGenerator/Defines.hpp"

namespace cct
{
	/**
	 * Context passed to generation hooks.
	 * Contains the current state of generation and output stream.
	 */
	struct GenerationContext
	{
		const Package* package = nullptr;
		const Namespace* currentNamespace = nullptr;
		const Class* currentClass = nullptr;
		const Enum* currentEnum = nullptr;
		const Enum::Element* currentEnumElement = nullptr;
		const Class::Member* currentMember = nullptr;
		const Class::Method* currentMethod = nullptr;
		const char* namespacePath = nullptr;
		std::ostream* outputStream = nullptr;
		std::size_t* indentLevel = nullptr;
		std::string_view apiMacro;
		uint64_t Seed = 0;
		void* pluginPrivateData = nullptr;
		const char* const* headers = nullptr;
		std::size_t headerCount = 0;

		template<typename... T>
		void Write(const std::format_string<T...> formatStr, T&&... args)
		{
			if (outputStream == nullptr || indentLevel == nullptr)
			{
				return;
			}
			using namespace std::string_view_literals;
			std::string data = std::vformat(formatStr.get(), std::make_format_args(args...));
			auto lines = data | std::ranges::views::split("\n"sv);

			for (auto line : lines)
			{
				*outputStream << std::string(*indentLevel, '\t');
				*outputStream << std::string_view(line.data(), line.size());
			}
			*outputStream << '\n';
		}

		void Write(std::string_view str)
		{
			if (outputStream == nullptr || indentLevel == nullptr)
				return;
			using namespace std::string_view_literals;
			auto lines = str | std::ranges::views::split("\n"sv);

			for (auto line : lines)
			{
				*outputStream << std::string(*indentLevel, '\t');
				*outputStream << std::string_view(line.data(), line.size());
			}
			*outputStream << '\n';
		}

		void NewLine() const
		{
			if (outputStream != nullptr)
			{
				*outputStream << '\n';
			}
		}

		void EnterScope() const
		{
			if (outputStream == nullptr || indentLevel == nullptr)
			{
				return;
			}
			*outputStream << std::string(*indentLevel, '\t') << "{\n";
			++(*indentLevel);
		}

		void LeaveScope(std::string_view str = "") const
		{
			if (outputStream == nullptr || indentLevel == nullptr)
			{
				return;
			}
			--(*indentLevel);
			*outputStream << std::string(*indentLevel, '\t') << "}" << str << "\n";
		}
	};

	/**
	 * Plugin information structure.
	 */
	struct PluginInfo
	{
		std::string_view name;
		std::string_view version;
		std::string_view author;
		std::string_view description;
	};
} // namespace cct

#endif // CONCERTO_PKGGENERATOR_REFLECTIONGENERATORPLUGIN_HPP
