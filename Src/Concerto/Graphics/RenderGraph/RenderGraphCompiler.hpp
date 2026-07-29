//
// Created by arthur on 11/05/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_RENDERGRAPH_COMPILER_HPP
#define CONCERTO_GRAPHICS_RHI_RENDERGRAPH_COMPILER_HPP

#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Concerto/Graphics/RenderGraph/RenderGraphPass.hpp"
#include "Concerto/Graphics/RenderGraph/RenderGraphResource.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"

namespace cct::gfx::rhi
{
	class Device;
	class RenderGraphResourceRegistry;

	class CONCERTO_GRAPHICS_RHI_BASE_API RenderGraphCompiler
	{
	public:
		std::vector<RGCompiledPass> Compile(
			std::vector<RGPass>& passes,
			RenderGraphResourceRegistry& registry,
			Device& device,
			RGTextureHandle finalOutput);

		// Returns the required ImageLayout for a given texture usage
		static ImageLayout RequiredLayout(const RGTextureUsage& usage);

		// Returns (srcStage, dstStage, srcAccess, dstAccess) for a layout transition
		static std::tuple<PipelineStageFlags, PipelineStageFlags, MemoryAccessFlags, MemoryAccessFlags>
		InferBarrierParams(ImageLayout oldLayout, ImageLayout newLayout);

		static std::pair<PipelineStageFlags, MemoryAccessFlags>
		BufferAccessParams(RGPassType type, RGResourceAccess access);

	private:
		struct PassDep
		{
			std::vector<UInt32> incoming; // passes this pass depends on
		};

		void BuildDependencies(std::vector<RGPass>& passes);
		void CullPasses(std::vector<RGPass>& passes, RGTextureHandle finalOutput);
		std::vector<UInt32> TopologicalSort(const std::vector<RGPass>& passes);
		void BuildRenderPassObjects(RGCompiledPass& compiled, RGPass& pass,
									UInt32 passIdx, const std::unordered_set<UInt16>& alreadyWritten,
									const RenderGraphResourceRegistry& registry, Device& device);

		std::unordered_map<UInt16, UInt32> m_textureLastWriter; // texIdx → last writer passIdx
		std::unordered_map<UInt16, UInt32> m_bufferLastWriter;
		std::vector<PassDep> m_deps;
		std::vector<std::vector<UInt32>> m_adjOut; // outgoing edges: adjOut[A] = {B,...} A→B
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_RENDERGRAPH_COMPILER_HPP
