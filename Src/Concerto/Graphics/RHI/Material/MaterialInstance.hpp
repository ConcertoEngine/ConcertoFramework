#ifndef CONCERTO_GRAPHICS_RHI_MATERIAL_MATERIALINSTANCE_HPP
#define CONCERTO_GRAPHICS_RHI_MATERIAL_MATERIALINSTANCE_HPP

#include <cstddef>
#include <memory>
#include <vector>

#include <Concerto/Core/Math/Vector/Vector.hpp>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Material.hpp"
#include "Concerto/Graphics/RHI/Material/MaterialTemplate.hpp"

namespace cct::gfx::rhi
{
	class DescriptorSet;
	class Texture;
	class Buffer;

	class CONCERTO_GRAPHICS_RHI_BASE_API MaterialInstance
	{
	public:
		MaterialInstance() = default;
		MaterialInstance(const MaterialInstance&) = delete;
		MaterialInstance& operator=(const MaterialInstance&) = delete;
		MaterialInstance(MaterialInstance&&) = default;
		MaterialInstance& operator=(MaterialInstance&&) = default;

		[[nodiscard]] std::size_t GetHash() const;

		void SetValue(const std::string& name, float value);
		void SetValue(const std::string& name, const Vector2f& value);
		void SetValue(const std::string& name, const Vector3f& value);
		void SetValue(const std::string& name, const Vector4f& value);
		void SetValue(const std::string& name, Int32 value);
		void SetValue(const std::string& name, UInt32 value);
		void SetValue(const std::string& name, bool value);

		void UploadValues();

		MaterialInfo info;
		MaterialTemplatePtr materialTemplate;
		std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;
		std::shared_ptr<Texture> diffuseTexture;
		std::shared_ptr<Texture> normalTexture;
		std::vector<std::byte> valueData;
		std::shared_ptr<Buffer> valueBuffer;
		bool valueDirty = false;

	private:
		void SetValueBytes(const std::string& name, const void* data, std::size_t size);
	};
	using MaterialInstancePtr = std::shared_ptr<MaterialInstance>;
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_MATERIAL_MATERIALINSTANCE_HPP
