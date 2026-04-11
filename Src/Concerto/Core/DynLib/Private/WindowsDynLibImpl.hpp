//
// Created by arthur on 02/06/2024.
//

#ifndef CONCERTO_CORE_WINDOWS_DYNLIBIMPL_HPP
#define CONCERTO_CORE_WINDOWS_DYNLIBIMPL_HPP

#include <windows.h>
#include <filesystem>
#include <string>
#include <string_view>

namespace cct
{
	class DynLibImpl
	{
	 public:
		DynLibImpl() = default;
		DynLibImpl(const DynLibImpl&) = delete;

		bool Load(const std::filesystem::path& path, std::string* error = nullptr);
		bool Unload(std::string* error = nullptr);
		void* GetSymbol(const std::string& symbol, std::string* error = nullptr) const;

	 private:
		HMODULE _module = {};
	};
}// namespace cct
#endif//CONCERTO_CORE_WINDOWS_DYNLIBIMPL_HPP