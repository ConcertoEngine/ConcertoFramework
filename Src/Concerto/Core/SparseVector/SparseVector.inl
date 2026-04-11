//
// Created by arthur on 12/10/2023.
//

#ifndef CONCERTO_CORE_SPARSEVECTOR_INL
#define CONCERTO_CORE_SPARSEVECTOR_INL

#include "Concerto/Core/Types/Types.hpp"
#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/SparseVector/SparseVector.hpp"

namespace cct
{
    template<typename ValueType, typename Allocator>
    SparseVector<ValueType, Allocator>::SparseVector(SparseVector::size_type size) :
        _container(size)
    {

    }

	template<typename ValueType, typename Allocator>
	SparseVector<ValueType, Allocator>::size_type SparseVector<ValueType, Allocator>::size() const
	{
		return _container.size();
	}

	template<typename ValueType, typename Allocator>
	[[nodiscard]] bool SparseVector<ValueType, Allocator>::empty() const
	{
		return _container.empty();
	}

	template<typename ValueType, typename Allocator>
	SparseVector<ValueType, Allocator>::iterator SparseVector<ValueType, Allocator>::begin()
	{
		return _container.begin();
	}

	template<typename ValueType, typename Allocator>
	SparseVector<ValueType, Allocator>::iterator SparseVector<ValueType, Allocator>::end()
	{
		return _container.end();
	}

	template<typename ValueType, typename Allocator>
	SparseVector<ValueType, Allocator>::const_iterator SparseVector<ValueType, Allocator>::begin() const
	{
		return _container.cbegin();
	}

	template<typename ValueType, typename Allocator>
	SparseVector<ValueType, Allocator>::const_iterator SparseVector<ValueType, Allocator>::end() const
	{
		return _container.cend();
	}

	template<typename ValueType, typename Allocator>
	SparseVector<ValueType, Allocator>::reference_type SparseVector<ValueType, Allocator>::operator[](size_type index)
	{
		if (index >= _container.size())
		{
			CCT_ASSERT_FALSE("Index out of range");
			throw std::out_of_range("Index out of range");
		}
		if (!Has(index))
			throw std::runtime_error("Index Has no value");
		return _container[index].value();
	}

	template<typename ValueType, typename Allocator>
	SparseVector<ValueType, Allocator>::const_reference_type SparseVector<ValueType, Allocator>::operator[](size_type index) const
	{
		if (index >= _container.size())
		{
			CCT_ASSERT_FALSE("Index out of range");
			throw std::out_of_range("Index out of range");
		}
		if (!Has(index))
		{
			CCT_ASSERT_FALSE("Index out of range");
			throw std::runtime_error("Index Has no value");
		}
		return _container[index].value();
	}

    template<typename ValueType, typename Allocator>
    template<typename... Args>
	SparseVector<ValueType, Allocator>::reference_type SparseVector<ValueType, Allocator>::Emplace(SparseVector::size_type index, Args&&... args)
    {
        if (index >= _container.size())
            _container.resize(index + 1);
        _container[index] = std::make_optional<ValueType>(std::forward<Args>(args)...);
        return _container.at(index).value();
    }

    template<typename ValueType, typename Allocator>
    void SparseVector<ValueType, Allocator>::Erase(SparseVector::size_type index)
    {
        CCT_ASSERT(index >= _container.size() - 1 || !_container[index].has_value(), "Invalid index");
        _container[index].reset();
    }

    template<typename ValueType, typename Allocator>
    bool SparseVector<ValueType, Allocator>::Has(SparseVector::size_type index) const
    {
        return index < _container.size() && _container[index].has_value();
    }

    template<typename ValueType, typename Allocator>
    void SparseVector<ValueType, Allocator>::Clear()
    {
        _container.clear();
    }
}

#endif //CONCERTO_CORE_SPARSEVECTOR_INL