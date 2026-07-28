//
// Created by arthur on 23/10/2025.
//

#ifndef CONCERTO_GRAPHICS_CORE_SHADERTYPES_HPP
#define CONCERTO_GRAPHICS_CORE_SHADERTYPES_HPP

#include <cstdint>

#include <Concerto/Core/EnumFlags/EnumFlags.hpp>

#include "Concerto/Graphics/Core/Defines.hpp"

namespace cct::gfx
{
	enum class ShaderBindingType
	{
		Sampler = 0,
		StorageBuffer = 1,
		ReadOnlyStorageBuffer = 2,
		Texture = 3,
		UniformBuffer = 4,
		UniformBufferDynamic = 5,
		CombinedImageSampler = 6,
		StorageImage = 7,

		Max = StorageImage
	};

	enum class ShaderStage : UInt32
	{
		None = 0,
		Vertex = 1 << 0,
		Fragment = 1 << 1,
		Compute = 1 << 2,

		All = Vertex | Fragment | Compute
	};

	using ShaderStageFlags = EnumFlags<ShaderStage>;
} // namespace cct::gfx

CCT_ENABLE_ENUM_FLAGS(cct::gfx::ShaderBindingType);
CCT_ENABLE_ENUM_FLAGS(cct::gfx::ShaderStage);

#endif // CONCERTO_GRAPHICS_CORE_SHADERTYPES_HPP
