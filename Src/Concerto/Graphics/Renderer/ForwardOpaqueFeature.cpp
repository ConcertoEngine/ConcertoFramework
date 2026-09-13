#include "Concerto/Graphics/Renderer/ForwardOpaqueFeature.hpp"

#include <Concerto/Core/Math/AABB/AABB.hpp>

#include "Concerto/Graphics/RenderGraph/RenderGraphBuilder.hpp"
#include "Concerto/Graphics/RenderGraph/RenderGraphContext.hpp"
#include "Concerto/Graphics/RHI/CommandBuffer.hpp"
#include "Concerto/Graphics/RHI/GpuSubMesh/GpuSubMesh.hpp"
#include "Concerto/Graphics/RHI/Material/MaterialInstance.hpp"

namespace cct::gfx
{
	namespace
	{
		void DrawMesh(rhi::RenderGraphContext& ctx, const rhi::GpuMesh& mesh, const Frustum& frustum)
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
			for (const auto& subMesh : mesh.subMeshes)
			{
				const auto& material = subMesh->GetMaterial();
				if (material == nullptr)
					continue;
				const AABB worldBounds = subMesh->GetLocalBounds().Transformed(mesh.transformMatrix);
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
				cmd.Draw(static_cast<UInt32>(subMesh->GetVertices().size()), 1, 0, 0);
			}
		}
	} // namespace

	void ForwardOpaqueFeature::SetMesh(rhi::GpuMeshPtr mesh)
	{
		m_mesh = std::move(mesh);
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
			{ DrawMesh(ctx, *m_mesh, m_frustum); });
	}

	void ForwardOpaqueFeature::UpdateFrameData(const View& view)
	{
		m_frustum = Frustum::FromViewProjection(view.projectionMatrix * view.viewMatrix);
	}
} // namespace cct::gfx
