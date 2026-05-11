//
// Created by arthur on 15/04/2023.
//

#include "Concerto/Graphics/Core/Primitives/Primitives.hpp"

#include <Concerto/Core/Assert.hpp>
#include <Concerto/Core/Math/Algorithm.hpp>
#include <Concerto/Core/Math/Vector/Vector.hpp>

#include "Concerto/Graphics/Core/Vertex.hpp"

namespace cct::gfx
{
	Vertices Primitive::MakeCone(float radius, float height, UInt32 slices)
	{
		Vertices vertices;
		const float angleStep = 2.0f * cct::Pi<float> / static_cast<float>(slices);
		const Vector3f baseCenter(0.0f, -0.5f * height, 0.0f);
		for (UInt32 i = 0; i < slices; ++i)
		{
			const float angle = static_cast<float>(i) * angleStep;
			const Vector3f baseVertex(radius * std::cos(angle), -0.5f * height, radius * std::sin(angle));
			const Vector3f baseNormal = (baseVertex - baseCenter).Cross(Vector3f(0.0f, 1.0f, 0.0f)).Normalize();
			vertices.push_back({baseVertex, baseNormal, Vector3f(1.0f, 1.0f, 1.0f), Vector2f(0.0f, 0.0f)});
		}

		const Vector3f tip(0.0f, 0.5f * height, 0.0f);
		const Vector3f tipNormal = (Vector3f(0.0f, 1.0f, 0.0f) - Vector3f(0.0f, 0.0f, 0.0f)).Normalize();
		vertices.push_back({tip, tipNormal, Vector3f(1.0f, 1.0f, 1.0f), Vector2f(0.0f, 0.0f)});

		const std::size_t baseIndexOffset = 0;
		const std::size_t tipIndex = vertices.size() - 1;
		for (UInt32 i = 0; i < slices; ++i)
		{
			const std::size_t j = (i + 1) % slices;
			const std::size_t baseIndex1 = baseIndexOffset + i;
			const std::size_t baseIndex2 = baseIndexOffset + j;
			vertices.push_back({vertices[baseIndex1].position, vertices[baseIndex1].normal,
								Vector3f(1.0f, 1.0f, 1.0f), Vector2f(0.0f, 0.0f)});
			vertices.push_back({vertices[baseIndex2].position, vertices[baseIndex2].normal,
								Vector3f(1.0f, 1.0f, 1.0f), Vector2f(0.0f, 0.0f)});
			vertices.push_back({tip, tipNormal, Vector3f(1.0f, 1.0f, 1.0f), Vector2f(0.0f, 0.0f)});
			const std::size_t tipIndexOffset = tipIndex + (i * 3);
			vertices[tipIndexOffset].uv = Vector2f(0.0f, 0.0f);
			vertices[tipIndexOffset + 1].uv = Vector2f(1.0f, 0.0f);
			vertices[tipIndexOffset + 2].uv = Vector2f(0.5f, 1.0f);
			vertices[tipIndexOffset].normal = (vertices[tipIndexOffset + 1].position - vertices[tipIndexOffset].position).Cross(vertices[tipIndexOffset + 2].position - vertices[tipIndexOffset].position).Normalize();
			vertices[tipIndexOffset + 1].normal = vertices[tipIndexOffset].normal;
			vertices[tipIndexOffset + 2].normal = vertices[tipIndexOffset].normal;
		}

		return vertices;
	}

	Vertices Primitive::MakeCube(float size)
	{
		Vertices vertices;
		const float halfSize = 0.5f * size;
		const Vector3f positions[8] =
			{
				Vector3f(-halfSize, -halfSize, -halfSize),
				Vector3f(-halfSize, -halfSize, halfSize),
				Vector3f(-halfSize, halfSize, -halfSize),
				Vector3f(-halfSize, halfSize, halfSize),
				Vector3f(halfSize, -halfSize, -halfSize),
				Vector3f(halfSize, -halfSize, halfSize),
				Vector3f(halfSize, halfSize, -halfSize),
				Vector3f(halfSize, halfSize, halfSize)};
		const Vector3f normals[6] =
			{
				Vector3f(-1.0f, 0.0f, 0.0f),
				Vector3f(1.0f, 0.0f, 0.0f),
				Vector3f(0.0f, -1.0f, 0.0f),
				Vector3f(0.0f, 1.0f, 0.0f),
				Vector3f(0.0f, 0.0f, -1.0f),
				Vector3f(0.0f, 0.0f, 1.0f)};
		const Vector2f uvs[4] =
			{
				Vector2f(0.0f, 0.0f),
				Vector2f(1.0f, 0.0f),
				Vector2f(1.0f, 1.0f),
				Vector2f(0.0f, 1.0f)};
		const UInt32 indices[36] =
			{
				0, 1, 2, 2, 1, 3,
				4, 6, 5, 5, 6, 7,
				0, 2, 4, 4, 2, 6,
				1, 5, 3, 3, 5, 7,
				0, 4, 1, 1, 4, 5,
				2, 3, 6, 6, 3, 7};
		for (auto index : indices)
		{
			const Vector3f position = positions[index];
			const Vector3f normal = normals[index / 4];
			const Vector2f uv = uvs[index % 4];
			vertices.push_back({position, normal, Vector3f(1.0f, 1.0f, 1.0f), uv});
		}
		return vertices;
	}

	Vertices Primitive::MakeSphere(float radius, UInt32 slices, UInt32 stacks)
	{
		Vertices vertices;

		const Vector3f top(0.0f, radius, 0.0f);
		const Vector3f bottom(0.0f, -radius, 0.0f);
		vertices.push_back({top, Vector3f(0.0f, 1.0f, 0.0f), Vector3f(1.0f, 1.0f, 1.0f), Vector2f(0.0f, 0.0f)});
		vertices.push_back({bottom, Vector3f(0.0f, -1.0f, 0.0f), Vector3f(1.0f, 1.0f, 1.0f),
							Vector2f(0.0f, 0.0f)});

		const float phiStep = cct::Pi<float> / static_cast<float>(stacks + 1);
		const float thetaStep = 2.0f * cct::Pi<float> / static_cast<float>(slices);
		for (UInt32 i = 1; i <= stacks; ++i)
		{
			const float phi = static_cast<float>(i) * phiStep;
			const float cosPhi = std::cos(phi);
			const float sinPhi = std::sin(phi);
			for (UInt32 j = 0; j < slices; ++j)
			{
				const float theta = static_cast<float>(j) * thetaStep;
				const float cosTheta = std::cos(theta);
				const float sinTheta = std::sin(theta);
				const Vector3f position(radius * sinPhi * cosTheta, radius * cosPhi, radius * sinPhi * sinTheta);
				const Vector3f normal = position.Normalize();
				vertices.push_back({position, normal, Vector3f(1.0f, 1.0f, 1.0f),
									Vector2f(static_cast<float>(j) / static_cast<float>(slices),
											 static_cast<float>(i) / static_cast<float>(stacks + 1))});
			}
		}

		for (UInt32 i = 0; i < slices; ++i)
		{
			const std::size_t j = (i + 1) % slices;
			const std::size_t index1 = 0;
			const std::size_t index2 = 2 + i;
			const std::size_t index3 = 2 + j;
			vertices.push_back({vertices[index1].position, vertices[index1].normal, Vector3f(1.0f, 1.0f, 1.0f),
								Vector2f(0.0f, 0.0f)});
			vertices.push_back({vertices[index2].position, vertices[index2].normal, Vector3f(1.0f, 1.0f, 1.0f),
								Vector2f(0.0f, 0.0f)});
			vertices.push_back({vertices[index3].position, vertices[index3].normal, Vector3f(1.0f, 1.0f, 1.0f),
								Vector2f(0.0f, 0.0f)});
		}
		for (UInt32 i = 0; i < stacks - 1; ++i)
		{
			for (UInt32 j = 0; j < slices; ++j)
			{
				const std::size_t index1 = 2 + i * slices + j;
				const std::size_t index2 = index1 + slices;
				const std::size_t index3 = (index1 + 1) % (slices * (stacks - 1)) + 2;
				const std::size_t index4 = (index2 + 1) % (slices * (stacks - 1)) + 2;
				vertices.push_back({vertices[index1].position, vertices[index1].normal, Vector3f(1.0f, 1.0f, 1.0f),
									vertices[index1].uv});
				vertices.push_back({vertices[index2].position, vertices[index2].normal, Vector3f(1.0f, 1.0f, 1.0f),
									vertices[index2].uv});
				vertices.push_back({vertices[index3].position, vertices[index3].normal, Vector3f(1.0f, 1.0f, 1.0f),
									vertices[index3].uv});
				vertices.push_back({vertices[index2].position, vertices[index2].normal, Vector3f(1.0f, 1.0f, 1.0f),
									vertices[index2].uv});
				vertices.push_back({vertices[index4].position, vertices[index4].normal, Vector3f(1.0f, 1.0f, 1.0f),
									vertices[index4].uv});
				vertices.push_back({vertices[index3].position, vertices[index3].normal, Vector3f(1.0f, 1.0f, 1.0f),
									vertices[index3].uv});
			}
		}

		for (UInt32 i = 0; i < slices; ++i)
		{
			const std::size_t j = (i + 1) % slices;
			const std::size_t index1 = 1;
			const std::size_t index2 = static_cast<std::size_t>(vertices.size()) - slices + i;
			const std::size_t index3 = static_cast<std::size_t>(vertices.size()) - slices + j;
			vertices.push_back({vertices[index1].position, vertices[index1].normal, Vector3f(1.0f, 1.0f, 1.0f),
								Vector2f(0.0f, 0.0f)});
			vertices.push_back({vertices[index3].position, vertices[index3].normal, Vector3f(1.0f, 1.0f, 1.0f),
								Vector2f(0.0f, 0.0f)});
			vertices.push_back({vertices[index2].position, vertices[index2].normal, Vector3f(1.0f, 1.0f, 1.0f),
								Vector2f(0.0f, 0.0f)});
		}

		return vertices;
	}

	Vertices Primitive::MakeCylinder(float radius, float height, UInt32 slices)
	{
		const float step = 2 * cct::Pi<float> / slices;

		Vertices vertices;

		const Vector3f top_center(0.0f, height / 2.0f, 0.0f);
		const Vector3f bottom_center(0.0f, -height / 2.0f, 0.0f);
		vertices.push_back({top_center, Vector3f(0.0f, 1.0f, 0.0f).Normalize(), Vector3f(1.0f, 1.0f, 1.0f),
							Vector2f(0.5f, 0.5f)});
		vertices.push_back({bottom_center, Vector3f(0.0f, -1.0f, 0.0f).Normalize(), Vector3f(1.0f, 1.0f, 1.0f),
							Vector2f(0.5f, 0.5f)});

		for (UInt32 i = 0; i < slices; ++i)
		{
			const float x = radius * std::cos(i * step);
			const float z = radius * std::sin(i * step);
			const Vector3f top_position(x, height / 2.0f, z);
			const Vector3f bottom_position(x, -height / 2.0f, z);
			const Vector3f normal_top = Vector3f(x, 0.0f, z).Normalize();
			const Vector3f normal_bottom = Vector3f(x, 0.0f, z).Normalize();
			vertices.push_back({top_position, normal_top, Vector3f(1.0f, 1.0f, 1.0f),
								Vector2f(static_cast<float>(i) / slices, 1.0f)});
			vertices.push_back({bottom_position, normal_bottom, Vector3f(1.0f, 1.0f, 1.0f),
								Vector2f(static_cast<float>(i) / slices, 0.0f)});
		}

		for (UInt32 i = 0; i < slices; ++i)
		{
			const std::size_t j = (i + 1) % slices;
			const std::size_t index1 = 0;
			const std::size_t index2 = 2 * i + 2;
			const std::size_t index3 = 2 * j + 2;
			vertices.push_back({vertices[index1].position, vertices[index1].normal, Vector3f(1.0f, 1.0f, 1.0f),
								Vector2f(0.5f, 0.5f)});
			vertices.push_back({vertices[index3].position, vertices[index3].normal, Vector3f(1.0f, 1.0f, 1.0f),
								Vector2f(0.5f + 0.5f * std::cos(j * step), 0.5f + 0.5f * std::sin(j * step))});
			vertices.push_back({vertices[index2].position, vertices[index2].normal, Vector3f(1.0f, 1.0f, 1.0f),
								Vector2f(0.5f + 0.5f * std::cos(i * step), 0.5f + 0.5f * std::sin(i * step))});
		}

		for (UInt32 i = 0; i < slices; ++i)
		{
			const std::size_t j = (i + 1) % slices;
			const std::size_t top_index1 = 1;
			const std::size_t top_index2 = 2 * i + 2;
			const std::size_t top_index3 = 2 * j + 2;
			const std::size_t bottom_index1 = 0;
			const std::size_t bottom_index2 = 2 * i + 3;
			const std::size_t bottom_index3 = 2 * j + 3;
			vertices.push_back({vertices[top_index1].position, Vector3f(vertices[top_index1].position.X(), 0.0f, vertices[top_index1].position.Z()).Normalize(), Vector3f(1.0f, 1.0f, 1.0f), Vector2f(0.5f, 0.5f)});
			vertices.push_back({vertices[top_index3].position, Vector3f(vertices[top_index3].position.X(), 0.0f, vertices[top_index3].position.Z()).Normalize(), Vector3f(1.0f, 1.0f, 1.0f), Vector2f(0.5f + 0.5f * std::cos(j * step), 0.5f + 0.5f * std::sin(j * step))});
			vertices.push_back({vertices[top_index2].position, Vector3f(vertices[top_index2].position.X(), 0.0f, vertices[top_index2].position.Z()).Normalize(), Vector3f(1.0f, 1.0f, 1.0f), Vector2f(0.5f + 0.5f * std::cos(i * step), 0.5f + 0.5f * std::sin(i * step))});
			vertices.push_back({vertices[bottom_index1].position, Vector3f(vertices[bottom_index1].position.X(), 0.0f, vertices[bottom_index1].position.Z()).Normalize(), Vector3f(1.0f, 1.0f, 1.0f), Vector2f(0.5f, 0.5f)});
			vertices.push_back({vertices[bottom_index2].position, Vector3f(vertices[bottom_index2].position.X(), 0.0f, vertices[bottom_index2].position.Z()).Normalize(), Vector3f(1.0f, 1.0f, 1.0f), Vector2f(0.5f + 0.5f * std::cos(i * step), 0.5f - height / (2.0f * radius))});
			vertices.push_back({vertices[bottom_index3].position, Vector3f(vertices[bottom_index3].position.X(), 0.0f, vertices[bottom_index3].position.Z()).Normalize(), Vector3f(1.0f, 1.0f, 1.0f), Vector2f(0.5f + 0.5f * std::cos(j * step), 0.5f - height / (2.0f * radius))});
		}

		return vertices;
	}

	Vertices Primitive::MakePlane(float /*size*/)
	{
		CCT_ASSERT_FALSE("Not implemented");
		return {};
	}
} // namespace cct::gfx
