//
// Created by arthur on 02/06/2024.
//

#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/Error/Error.hpp"

#ifdef CCT_PLATFORM_WINDOWS
#include "WindowsDynLibImpl.hpp"

namespace cct
{
	bool DynLibImpl::Load(const std::filesystem::path& path, std::string* error)
	{
		_module = LoadLibraryW(path.c_str());
		if (_module == nullptr)
		{
			if (error)
				*error = Error::GetLastSystemErrorString();
			CCT_ASSERT_FALSE("ConcertoCore: Couldn't load library '{}' error: {}", path.string(), Error::GetLastSystemErrorString());
			return false;
		}
		return true;
	}

	bool DynLibImpl::Unload(std::string* error)
	{
		if (_module == nullptr)
			return true;
		const BOOL res = FreeLibrary(_module);
		if (!res)
		{
			if (error)
				*error = Error::GetLastSystemErrorString();
			CCT_ASSERT(_module, "ConcertoCore: Couldn't free library");
			return false;
		}
		_module = nullptr;
		return true;
	}

	void* DynLibImpl::GetSymbol(const std::string& symbol, std::string* error) const
	{
		void* symbolPtr = reinterpret_cast<void*>(GetProcAddress(_module, symbol.c_str()));
		if (symbolPtr == nullptr)
		{
			if (error)
				*error = Error::GetLastSystemErrorString();
		}
		return symbolPtr;
	}
}// namespace cct
#endif