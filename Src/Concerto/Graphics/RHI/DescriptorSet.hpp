//
// Created by arthur on 24/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DESCRIPTORSET_HPP
#define CONCERTO_GRAPHICS_RHI_DESCRIPTORSET_HPP

#include <memory>
#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::rhi
{
	class DescriptorSetLayout;
	class Buffer;
	class Texture;

	/**
	 * @brief RHI wrapper for descriptor sets - binds actual resources
	 */
	class CONCERTO_GRAPHICS_RHI_BASE_API DescriptorSet
	{
	public:
		virtual ~DescriptorSet() = default;

		/**
		 * @brief Bind a buffer to a specific binding index
		 */
		virtual void BindBuffer(UInt32 binding, const Buffer& buffer, UInt32 offset = 0, UInt32 range = 0) = 0;

		/**
		 * @brief Bind a texture to a specific binding index
		 */
		virtual void BindTexture(UInt32 binding, const Texture& texture) = 0;

		/**
		 * @brief Get the descriptor set layout
		 */
		virtual const std::shared_ptr<DescriptorSetLayout>& GetLayout() const = 0;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_DESCRIPTORSET_HPP
