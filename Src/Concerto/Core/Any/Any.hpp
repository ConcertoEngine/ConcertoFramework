//
// Created by Arthur on 08/10/2025.
//

#ifndef CONCERTO_CORE_ANY_HPP
#define CONCERTO_CORE_ANY_HPP

#include <array>
#include <variant>
#include <type_traits>
#include <utility>

#include "Concerto/Core/Types/Types.hpp"
#include "Concerto/Core/Defines.hpp"

namespace cct
{
	class Any
	{
	public:
		Any() noexcept;
		~Any();

		Any(const Any& other);
		Any(Any&& other) noexcept;

		Any& operator=(const Any& other);
		Any& operator=(Any&& other) noexcept;

		template<typename T, typename... Args>
		Any& operator=(Args&&... args);

		template<typename T>
		Any& operator=(T&& data);

		void Reset();
		[[nodiscard]] bool HasValue() const noexcept;

		template<typename T>
		[[nodiscard]] bool Is() const noexcept;

		template<typename T>
		T As();

		template<typename T, typename... Args>
		static Any Make(Args&&... args);

		template<typename T, typename... Args>
		void Emplace(Args&&... args);

	private:
		// Small-buffer size (bytes)
		static constexpr std::size_t SmallBufferSize = 32;

		struct AlignedBuffer
		{
			alignas(std::max_align_t) std::array<std::byte, SmallBufferSize> data{};
		};

		using Storage = std::variant<AlignedBuffer, void*>;

		template<typename T>
		struct Traits
		{
			using Exposed = T;
			using Decayed = std::remove_reference_t<Exposed>;
			using Stored = std::conditional_t<std::is_reference_v<Exposed>, std::add_pointer_t<std::remove_reference_t<Exposed>>, Decayed>;

			static constexpr bool FitsInStack = (sizeof(Stored) <= SmallBufferSize) && (alignof(Stored) <= alignof(std::max_align_t));
		};

		void* StackPtr() noexcept;
		const void* StackPtr() const noexcept;

		void* DataPtr() noexcept;
		const void* DataPtr() const noexcept;

		using DestroyFn = void(*)(Any&);
		using CopyFn = void(*)(const Any&, Any&);
		using MoveFn = void(*)(Any&, Any&);

		template<typename Exposed>
		static void DestroyImpl(Any& a);

		template<typename Exposed>
		static void CopyImpl(const Any& src, Any& dst);

		template<typename Exposed>
		static void MoveImpl(Any& src, Any& dst);

		template<typename Exposed, typename... Args>
		void EmplaceImpl(Args&&... args);

		UInt64 m_typeId = 0;
		Storage m_storage = { AlignedBuffer() };
		DestroyFn m_destroy = nullptr;
		CopyFn m_copy = nullptr;
		MoveFn m_move = nullptr;
	};
}

#include "Concerto/Core/Any/Any.inl"

#endif // CONCERTO_CORE_ANY_HPP
