//
// Created by arthur on 13/09/2022.
//

#ifndef CONCERTO_GRAPHICS_MATERIAL_HPP
#define CONCERTO_GRAPHICS_MATERIAL_HPP

#include <memory>
#include <vector>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Material.hpp"
#include "Concerto/Graphics/RHI/Texture.hpp"

namespace cct::gfx::vk
{
	class DescriptorSet;
	class Pipeline;

	//struct CONCERTO_GRAPHICS_RHI_BASE_API VkMaterial : public rhi::MaterialInfo
	//{
	//	VkMaterial() = default;
	//	VkMaterial(VkMaterial&&other) = default;
	//	VkMaterial(VkMaterial&) = delete;
	//	VkMaterial& operator=(VkMaterial&& other) = default;
	//	VkMaterial& operator=(VkMaterial&) = delete;

	//	std::shared_ptr<Pipeline> pipeline;
	//	std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;
	//};
	//using VkMaterialPtr = std::shared_ptr<VkMaterial>;
}
#endif //CONCERTO_GRAPHICS_MATERIAL_HPP
