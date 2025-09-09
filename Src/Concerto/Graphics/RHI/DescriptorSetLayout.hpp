//
// Created by arthur on 24/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DESCRIPTORSETLAYOUT_HPP
#define CONCERTO_GRAPHICS_RHI_DESCRIPTORSETLAYOUT_HPP

#include <vector>
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/Core/ShaderModule.hpp"

namespace cct::gfx::rhi
{
	/**
	 * @brief RHI wrapper for descriptor set layouts - describes how resources are bound
	 */
	class CONCERTO_GRAPHICS_RHI_BASE_API DescriptorSetLayout
	{
	public:
		virtual ~DescriptorSetLayout() = default;

		/**
		 * @brief Get the bindings for this descriptor set layout
		 */
		virtual const std::vector<cct::gfx::DescriptorSetLayoutBinding>& GetBindings() const = 0;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_DESCRIPTORSETLAYOUT_HPP
