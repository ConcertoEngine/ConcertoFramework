//
// Created by arthur on 13/09/2022.
//

#ifndef CONCERTO_GRAPHICS_GPUDATA_HPP
#define CONCERTO_GRAPHICS_GPUDATA_HPP

#include <Concerto/Core/Math/Matrix/Matrix.hpp>
#include <Concerto/Core/Math/Vector/Vector.hpp>

namespace cct::gfx
{
	struct GPUCamera
	{
		Matrix4f viewMatrix;
		Matrix4f projectionMatrix;
		Matrix4f viewProjectionMatrix;
	};

	struct GPUObjectData
	{
		Matrix4f modelMatrix;
	};

	struct GPUSceneData
	{
		Vector4f fogColor; // w is for exponent
		Vector4f fogDistances; //x for min, y for max, zw unused.
		Vector4f ambientColor;
		Vector4f sunlightDirection; //w for sun power
		Vector4f sunlightColor;
	};

	struct Scene
	{
		GPUSceneData gpuSceneData;
		Vector4f clearColor;
	};
}
#endif //CONCERTO_GRAPHICS_GPUDATA_HPP
