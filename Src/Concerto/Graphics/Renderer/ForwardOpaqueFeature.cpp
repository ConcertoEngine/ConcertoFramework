#include "Concerto/Graphics/Renderer/ForwardOpaqueFeature.hpp"

#include <Concerto/Core/Math/AABB/AABB.hpp>

#include "Concerto/Graphics/Renderer/RenderScene.hpp"
#include "Concerto/Graphics/RenderGraph/RenderGraphBuilder.hpp"
#include "Concerto/Graphics/RenderGraph/RenderGraphContext.hpp"
#include "Concerto/Graphics/RHI/CommandBuffer.hpp"
#include "Concerto/Graphics/RHI/GpuSubMesh/GpuSubMesh.hpp"
#include "Concerto/Graphics/RHI/Material/MaterialInstance.hpp"

namespace cct::gfx
{
	namespace
	{
		void DrawInstances(rhi::RenderGraphContext& ctx, const std::vector<MeshInstance>& instances, const Frustum& frustum)
		{
			rhi::CommandBuffer& cmd = ctx.GetCommandBuffer();
			cmd.SetViewport({
				.x = 0.f,
				.y = 0.f,
				.width = static_cast<float>(ctx.GetWidth()),
				.height = static_cast<float>(ctx.GetHeight()),
				.minDepth = 0.f,
				.maxDepth = 1.f,
			});
			cmd.SetScissor({0, 0, ctx.GetWidth(), ctx.GetHeight()});

			std::size_t lastBoundMaterial = 0;
			bool lastBindSucceeded = false;
			for (std::size_t i = 0; i < instances.size(); ++i)
			{
				const MeshInstance& instance = instances[i];
				const UInt32 objectIndex = static_cast<UInt32>(i);
				for (const auto& subMesh : instance.mesh->subMeshes)
				{
					const auto& material = subMesh->GetMaterial();
					if (material == nullptr)
						continue;
					const AABB worldBounds = subMesh->GetLocalBounds().Transformed(instance.transform);
					if (!frustum.ContainsAABB(worldBounds))
						continue;
					const std::size_t materialHash = material->GetHash();
					if (lastBoundMaterial != materialHash)
					{
						lastBoundMaterial = materialHash;
						lastBindSucceeded = cmd.BindMaterial(*material);
					}
					if (!lastBindSucceeded)
						continue;
					cmd.BindVertexBuffer(subMesh->GetVertexBuffer());
					cmd.Draw(static_cast<UInt32>(subMesh->GetVertices().size()), 1, 0, objectIndex);
				}
			}
		}
	} // namespace

	ForwardOpaqueFeature::ForwardOpaqueFeature(const RenderScene& scene) :
		m_scene(scene)
	{
	}

	void ForwardOpaqueFeature::Setup(rhi::RenderGraph& graph, FrameResources& resources)
	{
		graph.AddGraphicsPass(
			"SceneOpaque",
			[&resources](rhi::RenderGraphBuilder& b)
			{
				resources.backbuffer = b.Write(resources.backbuffer);
				resources.depth = b.WriteDepth(resources.depth);
			},
			[this](rhi::RenderGraphContext& ctx)
			{ DrawInstances(ctx, m_scene.GetInstances(), m_frustum); });
	}

	void ForwardOpaqueFeature::UpdateFrameData(const View& view)
	{
		m_frustum = Frustum::FromViewProjection(view.projectionMatrix * view.viewMatrix);
	}
} // namespace cct::gfx
