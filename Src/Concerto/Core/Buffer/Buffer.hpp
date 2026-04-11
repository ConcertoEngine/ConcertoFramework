//
// Created by arthur on 27/05/2023.
//

#ifndef CONCERTO_CORE_BUFFER_HPP
#define CONCERTO_CORE_BUFFER_HPP

#include <vector>

#include "Concerto/Core/Types/Types.hpp"

namespace cct
{
	class CCT_CORE_PUBLIC_API Buffer
	{
	 public:
		using container_type = std::vector<Byte>;
		using allocator_type = container_type::allocator_type;
		using const_iterator = container_type::const_iterator;
		using const_reference = container_type::const_reference;
		using const_pointer = container_type::const_pointer;
		using const_reverse_iterator = container_type::const_reverse_iterator;
		using difference_type = container_type::difference_type;
		using iterator = container_type::iterator;
		using pointer = container_type::pointer;
		using reference = container_type::reference;
		using reverse_iterator = container_type::reverse_iterator;
		using size_type = container_type::size_type;
		using value_type = container_type::value_type;

		Buffer() = default;
		Buffer(size_type size);
		Buffer(const void* data, size_type size);
		Buffer(const Buffer&) = default;
		Buffer(Buffer&&) noexcept = default;
		~Buffer() = default;
		Buffer& operator=(const Buffer&) = default;
		Buffer& operator=(Buffer&&) noexcept = default;

		/**
		 * @brief Append data to the end of the buffer
		 * @param data Pointer to the data
		 * @param size Size of the data
		 */
		void Append(const void* data, size_type size);

		/**
		 * @brief Append data to the end of the buffer
		 * @param data Data to append
		 */
		void Append(const Buffer& data);

		/**
		 * @brief Insert data at the specified position
		 * @param pos Position to insert the data
		 * @param data Pointer to the data
		 * @param size Size of the data
		 */
		void Insert(const_iterator pos, const void* data, size_type size);

		/**
		 * @brief Insert data at the specified position
		 * @param pos Position to insert the data
		 * @param data Data to insert
		 */
		void Insert(const_iterator pos, const Buffer& data);

		/**
		 * @brief Erase data from the buffer
		 * @param pos Position to erase the data
		 * @param size Size of the data to erase
		 */
		void Erase(const_iterator pos, size_type size);

		/**
		 * @brief Erase data from the buffer between two positions
		 * @param start The start position to erase the data
		 * @param end The end position to erase the data
		 */
		void Erase(const_iterator start, const_iterator end);

		/**
		 * @brief Get the size of the buffer
		 * @return The size of the buffer
		 */
		[[nodiscard]] size_type Size() const;

		/**
		 * @brief Get the capacity of the buffer
		 * @return The capacity of the buffer
		 */
		[[nodiscard]] size_type Capacity() const;

		/**
		 * @brief Get the raw data of the buffer
		 * @return A pointer to the first byte of the buffer
		 */
		[[nodiscard]] pointer GetRawData();

		/**
		 * @brief Get the raw data of the buffer
		 * @return A pointer to the first byte of the buffer
		 */
		[[nodiscard]] const_pointer GetRawData() const;

		/**
		 * @brief Reserve memory for the buffer
		 * @param size Size of the memory to reserve
		 */
		void Reserve(size_type size);

		/**
		 * @brief Resize the buffer
		 * @param size New size of the buffer
		 * @param byte The byte to fill the buffer with, default is 0
		 */
		void Resize(size_type size, Byte byte = 0);

		/**
		 * @brief Clear the buffer
		 */
		void Clear();

		/**
		 * @brief Shrink the buffer to fit the data
		 */
		void ShrinkToFit();

		//STL functions
		[[nodiscard]] iterator begin() noexcept;
		[[nodiscard]] const_iterator begin() const noexcept;
		[[nodiscard]] const_iterator cbegin() const noexcept;
		[[nodiscard]] reverse_iterator rbegin() noexcept;
		[[nodiscard]] const_reverse_iterator crbegin() const noexcept;
		[[nodiscard]] const_reverse_iterator rbegin() const noexcept;
		[[nodiscard]] iterator end() noexcept;
		[[nodiscard]] const_iterator end() const noexcept;
		[[nodiscard]] const_iterator cend() const noexcept;
		[[nodiscard]] reverse_iterator rend() noexcept;
		[[nodiscard]] const_reverse_iterator rend() const noexcept;
		[[nodiscard]] const_reverse_iterator crend() const noexcept;
		[[nodiscard]] bool empty() const noexcept;
		[[nodiscard]] size_type size() const noexcept;

		bool operator==(const Buffer& rhs) const;
		bool operator!=(const Buffer& rhs) const;

	 private:
		container_type _data;
	};
}
#endif //CONCERTO_CORE_BUFFER_HPP
