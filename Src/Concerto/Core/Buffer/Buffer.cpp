//
// Created by arthur on 27/05/2023.
//

#include "Concerto/Core/Buffer/Buffer.hpp"

namespace cct
{

	Buffer::Buffer(size_type size) : _data(size)
	{

	}

	Buffer::Buffer(const void* data, size_type size) :
		_data(static_cast<const_pointer>(data),static_cast<const_pointer>(data) + size)
	{

	}

	void Buffer::Append(const void* data, size_type size)
	{
		_data.insert(_data.end(), static_cast<const_pointer>(data), static_cast<const_pointer>(data) + size);
	}

	void Buffer::Append(const Buffer& data)
	{
		_data.insert(_data.end(), data.begin(), data.end());
	}

	void Buffer::Insert(const_iterator pos, const void* data, size_type size)
	{
		_data.insert(pos, static_cast<const_pointer>(data), static_cast<const_pointer>(data) + size);
	}

	void Buffer::Insert(const_iterator pos, const Buffer& data)
	{
		_data.insert(pos, data.begin(), data.end());
	}

	void Buffer::Erase(const_iterator pos, size_type size)
	{
		_data.erase(pos, pos + size);
	}

	void Buffer::Erase(const_iterator start, const_iterator end)
	{
		_data.erase(start, end);
	}

	Buffer::size_type Buffer::Size() const
	{
		return _data.size();
	}

	Buffer::size_type Buffer::Capacity() const
	{
		return _data.capacity();
	}

	Buffer::pointer Buffer::GetRawData()
	{
		return _data.data();
	}

	Buffer::const_pointer Buffer::GetRawData() const
	{
		return _data.data();
	}

	void Buffer::Reserve(size_type size)
	{
		_data.reserve(size);
	}

	void Buffer::Resize(size_type size, Byte byte)
	{
		_data.resize(size, byte);
	}

	void Buffer::Clear()
	{
		_data.clear();
	}

	void Buffer::ShrinkToFit()
	{
		_data.shrink_to_fit();
	}

	Buffer::iterator Buffer::begin() noexcept
	{
		return _data.begin();
	}

	Buffer::const_iterator Buffer::begin() const noexcept
	{
		return _data.begin();
	}

	Buffer::const_iterator Buffer::cbegin() const noexcept
	{
		return _data.cbegin();
	}

	Buffer::reverse_iterator Buffer::rbegin() noexcept
	{
		return _data.rbegin();
	}

	Buffer::const_reverse_iterator Buffer::crbegin() const noexcept
	{
		return _data.crbegin();
	}

	Buffer::const_reverse_iterator Buffer::rbegin() const noexcept
	{
		return _data.rbegin();
	}

	Buffer::iterator Buffer::end() noexcept
	{
		return _data.end();
	}

	Buffer::const_iterator Buffer::end() const noexcept
	{
		return _data.end();
	}

	Buffer::const_iterator Buffer::cend() const noexcept
	{
		return _data.cend();
	}

	Buffer::reverse_iterator Buffer::rend() noexcept
	{
		return _data.rend();
	}

	Buffer::const_reverse_iterator Buffer::rend() const noexcept
	{
		return _data.rend();
	}

	Buffer::const_reverse_iterator Buffer::crend() const noexcept
	{
		return _data.crend();
	}

	bool Buffer::empty() const noexcept
	{
		return _data.empty();
	}

	Buffer::size_type Buffer::size() const noexcept
	{
		return _data.size();
	}

	bool Buffer::operator==(const Buffer& rhs) const
	{
		return _data == rhs._data;
	}

	bool Buffer::operator!=(const Buffer& rhs) const
	{
		return _data != rhs._data;
	}
}