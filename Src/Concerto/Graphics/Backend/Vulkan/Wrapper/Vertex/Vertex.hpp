//
// Created by arthur on 09/06/22.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_VULKAN_VERTEX_HPP
#define CONCERTO_GRAPHICS_BACKEND_VULKAN_VERTEX_HPP

#include <vector>

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"

namespace cct::gfx::vk
{
	struct VertexInputDescription
	{

		std::vector<VkVertexInputBindingDescription> bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;

		VkPipelineVertexInputStateCreateFlags flags = 0;
	};

	VertexInputDescription CONCERTO_GRAPHICS_VULKAN_BACKEND_API GetVertexDescription();

} // Concerto

#endif //CONCERTO_GRAPHICS_BACKEND_VULKAN_VERTEX_HPP
