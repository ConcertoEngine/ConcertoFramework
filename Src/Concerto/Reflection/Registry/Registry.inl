//
// Created by arthur on 20/12/2024.
//

#ifndef CONCERTO_REFLECTION_REGISTRY_INL
#define CONCERTO_REFLECTION_REGISTRY_INL

#include "Concerto/Reflection/Registry/Registry.hpp"

#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	template<typename T>
		requires(std::is_base_of_v<Object, T> && std::is_polymorphic_v<T>)
	std::unique_ptr<T> Registry::Create()
	{
		auto object = Allocate(T::GetClass());
		if (!object)
			return nullptr;
		return std::unique_ptr<T>(static_cast<T*>(object.release()));
	}
}

#endif // CONCERTO_REFLECTION_REGISTRY_INL