#include "Concerto/Graphics/RHI/Material/MaterialInstance.hpp"

#include <cstring>

#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RHI/DescriptorSet.hpp"

namespace cct::gfx::rhi
{
	std::size_t MaterialInstance::GetHash() const
	{
		return info.GetHash();
	}

	void MaterialInstance::SetValueBytes(const std::string& name, const void* data, std::size_t size)
	{
		if (!materialTemplate)
			return;

		const cct::gfx::MaterialValueProperty* property = materialTemplate->FindValueProperty(name);
		if (!property || property->offset + size > valueData.size())
			return;

		std::memcpy(valueData.data() + property->offset, data, size);
		valueDirty = true;
	}

	void MaterialInstance::SetValue(const std::string& name, float value)
	{
		SetValueBytes(name, &value, sizeof(value));
	}

	void MaterialInstance::SetValue(const std::string& name, const Vector2f& value)
	{
		SetValueBytes(name, &value, sizeof(value));
	}

	void MaterialInstance::SetValue(const std::string& name, const Vector3f& value)
	{
		SetValueBytes(name, &value, sizeof(value));
	}

	void MaterialInstance::SetValue(const std::string& name, const Vector4f& value)
	{
		SetValueBytes(name, &value, sizeof(value));
	}

	void MaterialInstance::SetValue(const std::string& name, Int32 value)
	{
		SetValueBytes(name, &value, sizeof(value));
	}

	void MaterialInstance::SetValue(const std::string& name, UInt32 value)
	{
		SetValueBytes(name, &value, sizeof(value));
	}

	void MaterialInstance::SetValue(const std::string& name, bool value)
	{
		UInt32 boolValue = value ? 1 : 0;
		SetValueBytes(name, &boolValue, sizeof(boolValue));
	}

	void MaterialInstance::UploadValues()
	{
		if (!valueBuffer || valueData.empty() || !materialTemplate)
			return;

		Byte* mapped = nullptr;
		if (valueBuffer->Map(&mapped))
		{
			std::memcpy(mapped, valueData.data(), valueData.size());
			valueBuffer->UnMap();
		}

		const auto& materialParams = materialTemplate->materialParams;
		if (materialParams.setIndex < descriptorSets.size())
			descriptorSets[materialParams.setIndex]->BindBuffer(materialParams.binding, *valueBuffer);

		valueDirty = false;
	}
} // namespace cct::gfx::rhi
