//
// Created by arthur on 29/05/2023.
//

#ifndef CONCERTO_CORE_STREAM_HPP
#define CONCERTO_CORE_STREAM_HPP

#include <cstddef>
#include <vector>
#include "Concerto/Core/Types/Types.hpp"

namespace cct
{
	class Buffer;

	class CCT_CORE_PUBLIC_API Stream
	{
	public:
		explicit Stream(std::size_t capacity = 0xFFF);
		Stream(const Stream&) = delete;
		Stream(Stream&&) noexcept = default;
		virtual ~Stream() = default;

		void Write(const Buffer& data);
		void Write(const void* data, std::size_t size);
		std::size_t Read(void* data, std::size_t size);

		[[nodiscard]] UInt64 GetCursorPos() const;
		[[nodiscard]] UInt64 GetSize() const;

		bool SetCursorPos(UInt64 pos);

		Stream& operator=(const Stream&) = delete;
		Stream& operator=(Stream&&) noexcept = default;

		template<typename T>
		Stream& operator<<(const T& data);

		template<typename T>
		Stream& operator>>(T& data);
	 protected:
		UInt64 _cursorPos;
		std::vector<Byte> _buffer;
	};
}

#include "Concerto/Core/Stream/Stream.inl"

#endif //CONCERTO_CORE_STREAM_HPP
