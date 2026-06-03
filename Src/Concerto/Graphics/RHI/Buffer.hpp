//
// Created by arthur on 03/09/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_HPP
#define CONCERTO_GRAPHICS_RHI_HPP

#include <span>
#include <type_traits>

#include <Concerto/Core/FunctionRef/FunctionRef.hpp>

#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::rhi
{
	class Texture;

	class CONCERTO_GRAPHICS_RHI_BASE_API Buffer
	{
	public:
		virtual ~Buffer() = default;

		virtual bool CopyTo(const Texture& texture) = 0;
		virtual bool Map(Byte** data) = 0;
		virtual void UnMap() = 0;

		template<typename T>
			requires std::is_trivially_copyable_v<T>
		void Write(T& object, UInt32 padding = 0);

		template<typename DestBuffer, typename SrcObj>
		void Write(std::span<SrcObj> objects, FunctionRef<void(DestBuffer& destBuffer, SrcObj& srcObj)>&& copyFunc, std::size_t padding = 0);
	};
} // namespace cct::gfx::rhi

#include "Concerto/Graphics/RHI/Buffer.inl"

#endif // CONCERTO_GRAPHICS_RHI_HPP