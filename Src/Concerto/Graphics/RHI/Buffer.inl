//
// Created by arthur on 03/09/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_INL
#define CONCERTO_GRAPHICS_RHI_INL

#include <cstring>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/RHI/Buffer.hpp"

namespace cct::gfx::rhi
{
	template <typename T>
		requires std::is_trivially_copyable_v<T>
	void Buffer::Write(T& object, UInt32 padding)
	{
		Byte* data = nullptr;
		if (Map(&data) == false)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Cannot map buffer");
			return;
		}
		data += padding;
		std::memcpy(data, &object, sizeof(T));
		UnMap();
	}

	template <typename DestBuffer, typename SrcObj>
	void Buffer::Write(std::span<SrcObj> objects, FunctionRef<void(DestBuffer& destBuffer, SrcObj& srcObj)>&& copyFunc, std::size_t padding)
	{
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
}

#endif //CONCERTO_GRAPHICS_RHI_INL