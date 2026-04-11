//
// Created by arthur on 08/02/23.
//

#ifndef CONCERTO_GRAPHICS_BUFFER_INL
#define CONCERTO_GRAPHICS_BUFFER_INL

#include <cstring>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Buffer/Buffer.hpp"

namespace cct::gfx::vk
{
	template <typename T>
	void Buffer::Copy(T& object, std::size_t padding)
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");

		Byte* data = nullptr;
		if(Map(&data) == false)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Cannot map buffer");
			return;
		}
		data += padding;
		std::memcpy(data, &object, sizeof(T));
		UnMap();
	}

	template <typename DestBuffer, typename SrcObj>
	void Buffer::Copy(std::vector<SrcObj>& objects,	std::function<void(DestBuffer& destBuffer, SrcObj& srcObj)>&& copyFunc, std::size_t padding)
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");

		Byte* data = nullptr;
		if (Map(&data) == false)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Cannot map buffer");
			return;
		}
		data += padding;
		auto* destBuffer = static_cast<DestBuffer*>(data);
		for (std::size_t i = 0; i < objects.size(); i++)
		{
			copyFunc(destBuffer[i], objects[i]);
		}
		UnMap();
	}

	template <typename T>
	T* Buffer::Map()
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");

		Byte* data = nullptr;
		if (Map(&data) == false)
			CCT_ASSERT_FALSE("ConcertoGraphics: Cannot map buffer");
		return reinterpret_cast<T*>(data);
	}
} // cct::gfx::vk
#endif //CONCERTO_GRAPHICS_BUFFER_INL