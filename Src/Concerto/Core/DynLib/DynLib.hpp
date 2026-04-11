//
// Created by arthur on 02/06/2023.
//

#ifndef CONCERTO_CORE_DYNLIB_HPP
#define CONCERTO_CORE_DYNLIB_HPP


#include <filesystem>
#include <memory>

#include "Concerto/Core/Types/Types.hpp"
#include "Concerto/Core/FunctionRef/FunctionRef.hpp"

#ifdef CCT_PLATFORM_WINDOWS
#define CONCERTO_DYNLIB_EXTENSION ".dll"
#elif defined(CCT_PLATFORM_MACOS)
#define CONCERTO_DYNLIB_EXTENSION ".dylib"
#elif defined(CCT_PLATFORM_POSIX)
#define CONCERTO_DYNLIB_EXTENSION ".so"
#endif

namespace cct
{
	class DynLibImpl;

	class CCT_CORE_PUBLIC_API DynLib
	{
	 public:
		DynLib() = default;
		DynLib(DynLib&&) = default;
		DynLib(const DynLib&) = delete;
		~DynLib();

		DynLib& operator=(const DynLib&) = delete;
		DynLib& operator=(DynLib&&) = default;

		bool IsLoaded() const;
		bool Load(const std::filesystem::path& path);
		bool Unload();
		void* GetSymbol(const std::string& symbol) const;

		template<typename ReturnValue, typename... Args>
		ReturnValue Invoke(const std::string& functionName, Args&&... args);

		template<typename ReturnValue, typename... Args>
		FunctionRef<ReturnValue(Args...)> GetFunction(const std::string& functionName);

		template<typename T>
		T* GetValue(const std::string& valueName);

	private:
		struct ImplDeleter {
			void operator()(void* impl) const;
		};
		std::unique_ptr<void, ImplDeleter> _impl;
		mutable std::string _lastError;
	};

}// namespace cct

#include "Concerto/Core/DynLib/DynLib.inl"

#endif//CONCERTO_CORE_DYNLIB_HPP