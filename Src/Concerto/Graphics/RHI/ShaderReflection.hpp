//
// Created by arthur on 23/08/2023.
//

#ifndef CONCERTO_GRAPHICS_INCLUDE_SHADERREFLECTION_HPP_
#define CONCERTO_GRAPHICS_INCLUDE_SHADERREFLECTION_HPP_

#include <unordered_map>
#include <vector>

#include "Concerto/Graphics/Core/Defines.hpp"
#include "Concerto/Graphics/Core/ShaderTypes.hpp"
#include <NZSL/Ast/Module.hpp>
#include <NZSL/Ast/RecursiveVisitor.hpp>
#include <NZSL/Math/FieldOffsets.hpp>

namespace cct::gfx
{
	struct CONCERTO_GRAPHICS_CORE_API RenderPipelineLayoutInfo
	{
		struct CONCERTO_GRAPHICS_CORE_API Binding
		{
			UInt32 setIndex = 0;
			UInt32 bindingIndex;
			UInt32 arraySize = 1;
			ShaderBindingType type;
			nzsl::ShaderStageTypeFlags shaderStageFlags;
		};

		std::vector<Binding> bindings;
	};
} // namespace cct::gfx

#endif // CONCERTO_GRAPHICS_INCLUDE_SHADERREFLECTION_HPP_