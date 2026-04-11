//
// Created by arthur on 29/05/2023.
//

#include <cstring>

#include "Concerto/Core/Stream/Stream.hpp"
#include "Concerto/Core/Buffer/Buffer.hpp"

namespace cct
{
	Stream::Stream(std::size_t capacity) : _cursorPos(0)
	{
		_buffer.reserve(capacity);
	}

	void Stream::Write(const Buffer& data)
	{
		Write(data.GetRawData(), data.Size());
	}

	void Stream::Write(const void* data, std::size_t size)
	{
		if (_buffer.size() < _cursorPos + size)
			_buffer.resize(_cursorPos + size);
		std::memcpy(_buffer.data() + _cursorPos, data, size);
		_cursorPos += size;
	}

	std::size_t Stream::Read(void* data, std::size_t size)
	{
		if (_buffer.size() < _cursorPos + size)
			size = _buffer.size() - _cursorPos;
		std::memcpy(data, _buffer.data() + _cursorPos, size);
		_cursorPos += size;
		return size;
	}

	UInt64 Stream::GetCursorPos() const
	{
		return _cursorPos;
	}

	UInt64 Stream::GetSize() const
	{
		return _buffer.size();
	}

	bool Stream::SetCursorPos(UInt64 pos)
	{
		if (pos > _buffer.size())
			return false;
		_cursorPos = pos;
		return true;
	}
}