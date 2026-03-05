//
// Created by arthur on 17/02/2023.
//

#ifndef CONCERTO_GRAPHICS_INCLUDE_MATERIAL_HPP_
#define CONCERTO_GRAPHICS_INCLUDE_MATERIAL_HPP_

#include <string>
#include <memory>

#include <Concerto/Core/Math/Vector/Vector.hpp>

#include "Concerto/Graphics/RHI/Texture.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API MaterialInfo
	{
	public:
		struct CONCERTO_GRAPHICS_RHI_BASE_API Hash
		{
			std::size_t operator()(const MaterialInfo& material) const
			{
				std::size_t h1 = std::hash<std::string>{}(material.diffuseTexturePath);
				std::size_t h2 = std::hash<std::string>{}(material.normalTexturePath);
				return h1 ^ (h2 << 1);
			}
		};
		MaterialInfo() = default;
		bool operator==(const MaterialInfo& other) const
		{
			return diffuseTexture == other.diffuseTexture && diffuseColor == other.diffuseColor
				&& metallic == other.metallic && specular == other.specular && roughness == other.roughness
				&& anisotropy == other.anisotropy && emissiveColor == other.emissiveColor
				&& normalTexture == other.normalTexture;
		}

		[[nodiscard]] std::size_t GetHash() const
		{
			return Hash()(*this);
		}

		std::shared_ptr<Texture> diffuseTexture;
		std::string diffuseTexturePath;
		Vector3f diffuseColor;
		float metallic = 0.0f;
		Vector3f specular;
		float roughness = 0.0f;
		float anisotropy = 0.0f;
		Vector3f emissiveColor;
		std::shared_ptr<Texture> normalTexture;
		std::string normalTexturePath;
		std::string name;
		std::string vertexShaderPath;
		std::string fragmentShaderPath;
	};
	using MaterialPtr = std::shared_ptr<rhi::MaterialInfo>;
}
#endif //CONCERTO_GRAPHICS_INCLUDE_MATERIAL_HPP_
