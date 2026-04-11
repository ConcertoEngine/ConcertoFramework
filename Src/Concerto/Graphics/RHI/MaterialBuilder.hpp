//
// Created by arthur on 07/08/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_MATERILABUILDER_HPP
#define CONCERTO_GRAPHICS_RHI_MATERILABUILDER_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Material.hpp"
#include "Concerto/Graphics/RHI/Buffer.hpp"

namespace cct::gfx::rhi
{
	class Device;
	class RenderPass;

	class CONCERTO_GRAPHICS_RHI_BASE_API MaterialBuilder
	{
	public:
		virtual ~MaterialBuilder() = default;

		virtual MaterialPtr BuildMaterial(MaterialInfo& material, const RenderPass& renderPass) = 0;
		virtual void Update(const rhi::Buffer& buffer, UInt32 setIndex, UInt32 bindingIndex) = 0;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_MATERILABUILDER_HPP
