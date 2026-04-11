//
// Created by arthur on 09/06/22.
//

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Vertex/Vertex.hpp"
#include "Concerto/Graphics/Core/Vertex.hpp"

namespace cct::gfx::vk
{
	VertexInputDescription GetVertexDescription()
	{
		VertexInputDescription description {};

		//we will have just 1 vertex buffer binding, with a per-vertex rate
		VkVertexInputBindingDescription mainBinding = {};
		mainBinding.binding = 0;
		mainBinding.stride = sizeof(Vertex);
		mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		description.bindings.push_back(mainBinding);

		//_position will be stored at Location 0
		VkVertexInputAttributeDescription positionAttribute = {};
		positionAttribute.binding = 0;
		positionAttribute.location = 0;
		positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		positionAttribute.offset = offsetof(Vertex, position);

		//Normal will be stored at Location 1
		VkVertexInputAttributeDescription normalAttribute = {};
		normalAttribute.binding = 0;
		normalAttribute.location = 1;
		normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		normalAttribute.offset = offsetof(Vertex, normal);

		//Color will be stored at Location 2
		VkVertexInputAttributeDescription colorAttribute = {};
		colorAttribute.binding = 0;
		colorAttribute.location = 2;
		colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		colorAttribute.offset = offsetof(Vertex, color);

		//UV will be stored at Location 3
		VkVertexInputAttributeDescription uvAttribute = {};
		uvAttribute.binding = 0;
		uvAttribute.location = 3;
		uvAttribute.format = VK_FORMAT_R32G32_SFLOAT;
		uvAttribute.offset = offsetof(Vertex, uv);

		description.attributes.push_back(positionAttribute);
		description.attributes.push_back(normalAttribute);
		description.attributes.push_back(colorAttribute);
		description.attributes.push_back(uvAttribute);
		return description;
	}
}