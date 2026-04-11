//
// Created by arthur on 02/06/2024.
//
#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/Error/Error.hpp"

#ifdef CCT_PLATFORM_POSIX
#include "PosixDynLibImpl.hpp"

namespace cct
{
	bool DynLibImpl::Load(const std::filesystem::path& path, std::string* error)
	{
		std::error_code ec = {};
		auto fullPath = std::filesystem::canonical(path, ec);
		if (ec)
			return false;
		_module = dlopen(fullPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
		if (_module == nullptr)
		{
			if (error)
				*error = Error::GetLastSystemErrorString();
			CCT_ASSERT_FALSE("ConcertoCore: Couldn't load library '{}' error: {}", path.string(), dlerror());
			return false;
		}
		return true;
	}

	bool DynLibImpl::Unload(std::string* error)
	{
		if (_module == nullptr)
		{
			CCT_ASSERT_FALSE("ConcertoCore: Library handle must be valid");
			return false;
		}

		const int res = dlclose(_module);
		if (res != 0)
		{
			if (error)
				*error = dlerror();
			CCT_ASSERT(_module, "ConcertoCore: Couldn't free library '{}'", dlerror());
			return false;
		}
		_module = nullptr;
		return true;
	}

	void* DynLibImpl::GetSymbol(const std::string& symbol, std::string* error) const
	{
		void* symbolPtr = dlsym(_module, symbol.c_str());
		if (symbolPtr == nullptr)
		{
			if (error)
				*error = dlerror();
		}
		return symbolPtr;
	}
}// namespace cct
#endif