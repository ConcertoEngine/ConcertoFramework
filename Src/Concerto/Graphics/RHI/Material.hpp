//
// Created by arthur on 17/02/2023.
//

#ifndef CONCERTO_GRAPHICS_INCLUDE_MATERIAL_HPP_
#define CONCERTO_GRAPHICS_INCLUDE_MATERIAL_HPP_

#include <string>

#include <Concerto/Core/Math/Vector/Vector.hpp>

#include "Concerto/Graphics/RHI/Enums.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API MaterialInfo
	{
	public:
		struct CONCERTO_GRAPHICS_RHI_BASE_API Hash
		{
			std::size_t operator()(const MaterialInfo& material) const
			{
				std::size_t hash = 0;
				Combine(hash, material.diffuseTexturePath);
				Combine(hash, material.normalTexturePath);
				Combine(hash, material.diffuseColor.X());
				Combine(hash, material.diffuseColor.Y());
				Combine(hash, material.diffuseColor.Z());
				Combine(hash, material.metallic);
				Combine(hash, material.specular.X());
				Combine(hash, material.specular.Y());
				Combine(hash, material.specular.Z());
				Combine(hash, material.roughness);
				Combine(hash, material.anisotropy);
				Combine(hash, material.emissiveColor.X());
				Combine(hash, material.emissiveColor.Y());
				Combine(hash, material.emissiveColor.Z());
				return hash;
			}

		private:
			template<typename T>
			static void Combine(std::size_t& hash, const T& value)
			{
				hash ^= std::hash<T>{}(value) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			}
		};
		MaterialInfo() = default;
		bool operator==(const MaterialInfo& other) const
		{
			return diffuseTexturePath == other.diffuseTexturePath && diffuseColor == other.diffuseColor && metallic == other.metallic && specular == other.specular && roughness == other.roughness && anisotropy == other.anisotropy && emissiveColor == other.emissiveColor && normalTexturePath == other.normalTexturePath;
		}

		[[nodiscard]] std::size_t GetHash() const
		{
			return Hash()(*this);
		}

		std::string diffuseTexturePath;
		Vector3f diffuseColor;
		float metallic = 0.0f;
		Vector3f specular;
		float roughness = 0.0f;
		float anisotropy = 0.0f;
		Vector3f emissiveColor;
		std::string normalTexturePath;
		std::string name;
		std::string vertexShaderPath;
		std::string fragmentShaderPath;
		PipelineConfig pipelineConfig;
	};
} // namespace cct::gfx::rhi
#endif // CONCERTO_GRAPHICS_INCLUDE_MATERIAL_HPP_
