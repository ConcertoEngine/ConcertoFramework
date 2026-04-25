//
// Created by arthur
//

#ifndef CONCERTO_PKGGENERATOR_CHUNKEDSTREAMBUF_HPP
#define CONCERTO_PKGGENERATOR_CHUNKEDSTREAMBUF_HPP

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <deque>
#include <functional>
#include <iterator>
#include <memory>
#include <mutex>
#include <ostream>
#include <streambuf>
#include <thread>
#include <utility>
#include <vector>

#include <Concerto/Profiler/Profiler.hpp>

namespace cct
{
	/**
	 * A streambuf that stores data in fixed-size, non-contiguous chunks.
	 * Avoids the contiguous allocation and geometric reallocation of std::ostringstream.
	 * Memory usage is always within one chunk of the actual data size.
	 *
	 * Usage:
	 *     ChunkedStreamBuf buf;
	 *     std::ostream stream(&buf);
	 *     stream << "hello";
	 *     buf.WriteTo(outFile);
	 */
	class ChunkedStreamBuf : public std::streambuf
	{
	public:
		static constexpr std::size_t DefaultChunkSize = 256 * 1024; // 256 KB

		explicit ChunkedStreamBuf(std::size_t chunkSize = DefaultChunkSize) :
			m_chunkSize(chunkSize > 0 ? chunkSize : DefaultChunkSize),
			m_fullChunkBytes(0)
		{
			AllocateChunk();
		}

		[[nodiscard]] std::size_t GetTotalBytes() const noexcept
		{
			return m_fullChunkBytes + static_cast<std::size_t>(pptr() - pbase());
		}

		[[nodiscard]] bool Empty() const noexcept
		{
			return GetTotalBytes() == 0;
		}

		void WriteTo(std::ostream& out) const
		{
			for (std::size_t i = 0; i < m_chunks.size(); ++i)
			{
				std::size_t size = (i + 1 < m_chunks.size())
									   ? m_chunkSize
									   : static_cast<std::size_t>(pptr() - pbase());
				if (size > 0)
					out.write(m_chunks[i].data(), static_cast<std::streamsize>(size));
			}
		}

		struct ChunkView
		{
			const char* data;
			std::size_t size;
		};

		class Iterator
		{
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = ChunkView;
			using difference_type = std::ptrdiff_t;
			using pointer = const ChunkView*;
			using reference = ChunkView;

			Iterator(const ChunkedStreamBuf* buf, std::size_t index) :
				m_buf(buf),
				m_index(index)
			{
			}

			ChunkView operator*() const
			{
				const auto& chunk = m_buf->m_chunks[m_index];
				std::size_t size = (m_index + 1 < m_buf->m_chunks.size())
									   ? m_buf->m_chunkSize
									   : static_cast<std::size_t>(m_buf->pptr() - m_buf->pbase());
				return {chunk.data(), size};
			}

			Iterator& operator++()
			{
				++m_index;
				return *this;
			}
			Iterator operator++(int)
			{
				Iterator tmp = *this;
				++m_index;
				return tmp;
			}
			bool operator==(const Iterator& other) const
			{
				return m_buf == other.m_buf && m_index == other.m_index;
			}
			bool operator!=(const Iterator& other) const
			{
				return !(*this == other);
			}

		private:
			const ChunkedStreamBuf* m_buf;
			std::size_t m_index;
		};

		[[nodiscard]] Iterator begin() const
		{
			return Iterator(this, 0);
		}
		[[nodiscard]] Iterator end() const
		{
			return Iterator(this, m_chunks.size());
		}

	protected:
		int_type overflow(int_type ch) override
		{
			if (ch == traits_type::eof())
				return traits_type::not_eof(ch);

			m_fullChunkBytes += m_chunkSize;
			AllocateChunk();
			*pptr() = static_cast<char>(ch);
			pbump(1);
			return ch;
		}

		std::streamsize xsputn(const char* s, std::streamsize count) override
		{
			std::streamsize remaining = count;
			while (remaining > 0)
			{
				std::streamsize available = epptr() - pptr();
				if (available == 0)
				{
					m_fullChunkBytes += m_chunkSize;
					AllocateChunk();
					available = static_cast<std::streamsize>(m_chunkSize);
				}

				std::streamsize toWrite = std::min(remaining, available);
				std::memcpy(pptr(), s, static_cast<std::size_t>(toWrite));
				pbump(static_cast<int>(toWrite));
				s += toWrite;
				remaining -= toWrite;
			}
			return count;
		}

	private:
		void AllocateChunk()
		{
			m_chunks.emplace_back(m_chunkSize);
			auto& chunk = m_chunks.back();
			setp(chunk.data(), chunk.data() + m_chunkSize);
		}

		std::deque<std::vector<char>> m_chunks;
		std::size_t m_chunkSize;
		std::size_t m_fullChunkBytes;
	};

	/**
	 * Convenience ostream that owns a ChunkedStreamBuf.
	 *
	 * Usage:
	 *     ChunkedOStream stream;
	 *     stream << "data";
	 *     stream.WriteTo(outFile);
	 */
	class ChunkedOStream : public std::ostream
	{
	public:
		explicit ChunkedOStream(std::size_t chunkSize = ChunkedStreamBuf::DefaultChunkSize) :
			std::ostream(&m_buf),
			m_buf(chunkSize)
		{
		}

		[[nodiscard]] ChunkedStreamBuf& GetBuffer() noexcept
		{
			return m_buf;
		}
		[[nodiscard]] const ChunkedStreamBuf& GetBuffer() const noexcept
		{
			return m_buf;
		}
		[[nodiscard]] std::size_t GetTotalBytes() const noexcept
		{
			return m_buf.GetTotalBytes();
		}
		[[nodiscard]] bool Empty() const noexcept
		{
			return m_buf.GetTotalBytes() == 0;
		}

		void WriteTo(std::ostream& out) const
		{
			m_buf.WriteTo(out);
		}

	private:
		ChunkedStreamBuf m_buf;
	};

	/**
	 * A streambuf that flushes completed chunks asynchronously via a background thread.
	 *
	 * When a chunk is full, it is moved to a flush queue and written to the destination
	 * by a dedicated thread, while the writer thread continues filling the next chunk.
	 * Chunks are recycled after flushing to avoid repeated allocations.
	 *
	 * Backpressure: if the flush queue reaches maxQueuedChunks, the writer blocks
	 * until the flush thread drains a chunk.
	 *
	 * Usage:
	 *     std::ofstream file("output.cpp");
	 *     AsyncChunkedStreamBuf buf(file);
	 *     std::ostream stream(&buf);
	 *     stream << generatedCode;
	 *     // on destruction, remaining data is flushed and thread is joined
	 *
	 * Or with a custom callback (e.g. socket):
	 *     AsyncChunkedStreamBuf buf([&](const char* data, std::size_t size) {
	 *         send(socket, data, size, 0);
	 *     });
	 *
	 * The flush callback must not throw.
	 */
	class AsyncChunkedStreamBuf : public std::streambuf
	{
	public:
		using FlushCallback = std::function<void(const char*, std::size_t)>;

		static constexpr std::size_t DefaultChunkSize = 256 * 1024; // 256 KB
		static constexpr std::size_t DefaultMaxQueuedChunks = 8; // 2 MB max in-flight

		explicit AsyncChunkedStreamBuf(
			FlushCallback callback,
			std::size_t chunkSize = DefaultChunkSize,
			std::size_t maxQueuedChunks = DefaultMaxQueuedChunks) :
			m_chunkSize(chunkSize > 0 ? chunkSize : DefaultChunkSize),
			m_maxQueuedChunks(maxQueuedChunks > 0 ? maxQueuedChunks : DefaultMaxQueuedChunks),
			m_callback(std::move(callback))
		{
			m_currentChunk.resize(m_chunkSize);
			setp(m_currentChunk.data(), m_currentChunk.data() + m_chunkSize);
			m_flushThread = std::jthread([this](const std::stop_token&)
										 { FlushLoop(); });
		}

		explicit AsyncChunkedStreamBuf(
			std::shared_ptr<std::ostream> dest,
			std::size_t chunkSize = DefaultChunkSize,
			std::size_t maxQueuedChunks = DefaultMaxQueuedChunks) :
			AsyncChunkedStreamBuf(
				[dest = std::move(dest)](const char* data, std::size_t size)
				{
					CCT_AUTO_PROFILER_SCOPE();
					if (dest)
						dest->write(data, static_cast<std::streamsize>(size));
				},
				chunkSize,
				maxQueuedChunks)
		{
		}

		explicit AsyncChunkedStreamBuf(
			std::ostream& dest,
			std::size_t chunkSize = DefaultChunkSize,
			std::size_t maxQueuedChunks = DefaultMaxQueuedChunks) :
			AsyncChunkedStreamBuf(
				[&dest](const char* data, std::size_t size)
				{
					CCT_AUTO_PROFILER_SCOPE();
					dest.write(data, static_cast<std::streamsize>(size));
				},
				chunkSize,
				maxQueuedChunks)
		{
		}

		~AsyncChunkedStreamBuf() override
		{
			{
				std::unique_lock lock(m_mutex);
				auto bytes = static_cast<std::size_t>(pptr() - pbase());
				if (bytes > 0)
				{
					m_flushQueue.push_back({.m_buffer = std::move(m_currentChunk), .m_written = bytes});
				}
				m_done = true;
			}
			m_flushCV.notify_one();
		}

		AsyncChunkedStreamBuf(const AsyncChunkedStreamBuf&) = delete;
		AsyncChunkedStreamBuf& operator=(const AsyncChunkedStreamBuf&) = delete;
		AsyncChunkedStreamBuf(AsyncChunkedStreamBuf&&) = delete;
		AsyncChunkedStreamBuf& operator=(AsyncChunkedStreamBuf&&) = delete;

		[[nodiscard]] std::size_t GetTotalBytes() const noexcept
		{
			return m_submittedBytes.load(std::memory_order_relaxed) + static_cast<std::size_t>(pptr() - pbase());
		}

	protected:
		int_type overflow(int_type ch) override
		{
			if (ch == traits_type::eof())
				return traits_type::not_eof(ch);

			SubmitCurrentChunk(m_chunkSize);
			AcquireNewChunk();
			*pptr() = static_cast<char>(ch);
			pbump(1);
			return ch;
		}

		std::streamsize xsputn(const char* s, std::streamsize count) override
		{
			std::streamsize remaining = count;
			while (remaining > 0)
			{
				auto available = static_cast<std::streamsize>(epptr() - pptr());
				if (available == 0)
				{
					SubmitCurrentChunk(m_chunkSize);
					AcquireNewChunk();
					available = static_cast<std::streamsize>(m_chunkSize);
				}

				std::streamsize toWrite = std::min(remaining, available);
				std::memcpy(pptr(), s, static_cast<std::size_t>(toWrite));
				pbump(static_cast<int>(toWrite));
				s += toWrite;
				remaining -= toWrite;
			}
			return count;
		}

		int sync() override
		{
			auto bytes = static_cast<std::size_t>(pptr() - pbase());
			if (bytes > 0)
			{
				SubmitCurrentChunk(bytes);
				AcquireNewChunk();
			}
			// Wait for all queued chunks to be flushed
			{
				std::unique_lock lock(m_mutex);
				m_drainCV.wait(lock, [this]
							   { return m_flushQueue.empty() && !m_flushing; });
			}
			return 0;
		}

	private:
		struct ChunkEntry
		{
			std::vector<char> m_buffer;
			std::size_t m_written = 0;
		};

		void SubmitCurrentChunk(std::size_t validBytes)
		{
			CCT_AUTO_PROFILER_SCOPE();
			std::unique_lock lock(m_mutex);
			// Backpressure: block if flush queue is full
			m_writerCV.wait(lock, [this]
							{ return m_flushQueue.size() < m_maxQueuedChunks; });
			m_flushQueue.push_back({.m_buffer = std::move(m_currentChunk), .m_written = validBytes});
			m_submittedBytes.fetch_add(validBytes, std::memory_order_relaxed);
			m_flushCV.notify_one();
		}

		void AcquireNewChunk()
		{
			CCT_AUTO_PROFILER_SCOPE();
			std::lock_guard lock(m_mutex);
			if (!m_freePool.empty())
			{
				m_currentChunk = std::move(m_freePool.back());
				m_freePool.pop_back();
			}
			else
			{
				m_currentChunk.resize(m_chunkSize);
			}
			setp(m_currentChunk.data(), m_currentChunk.data() + m_chunkSize);
		}

		void FlushLoop()
		{
			while (true)
			{
				ChunkEntry entry;
				{
					std::unique_lock lock(m_mutex);
					m_flushCV.wait(lock, [this]
								   { return !m_flushQueue.empty() || m_done; });
					CCT_AUTO_PROFILER_SCOPE();
					if (m_flushQueue.empty() && m_done)
					{
						return;
					}

					if (m_flushQueue.empty())
					{
						continue;
					}

					entry = std::move(m_flushQueue.front());
					m_flushQueue.pop_front();
					m_flushing = true;
				}

				m_callback(entry.m_buffer.data(), entry.m_written);

				{
					std::lock_guard lock(m_mutex);
					m_flushing = false;
					m_freePool.push_back(std::move(entry.m_buffer));
					m_writerCV.notify_one();
					m_drainCV.notify_all();
				}
			}
		}

		std::size_t m_chunkSize;
		std::size_t m_maxQueuedChunks;
		FlushCallback m_callback;

		std::vector<char> m_currentChunk;
		std::deque<ChunkEntry> m_flushQueue;
		std::vector<std::vector<char>> m_freePool;
		std::mutex m_mutex;
		std::condition_variable m_flushCV; // wake flush thread
		std::condition_variable m_writerCV; // backpressure relief
		std::condition_variable m_drainCV; // sync() drain notification
		bool m_done = false;
		bool m_flushing = false;
		std::atomic<std::size_t> m_submittedBytes{0};

		std::jthread m_flushThread;
	};

	/**
	 * Convenience ostream that owns an AsyncChunkedStreamBuf.
	 *
	 * Usage:
	 *     std::ofstream file("output.cpp");
	 *     AsyncChunkedOStream stream(file);
	 *     stream << generatedCode;
	 *     stream.flush();  // waits for all data to be written
	 */
	class AsyncChunkedOStream : public std::ostream
	{
	public:
		explicit AsyncChunkedOStream(
			AsyncChunkedStreamBuf::FlushCallback callback,
			std::size_t chunkSize = AsyncChunkedStreamBuf::DefaultChunkSize,
			std::size_t maxQueuedChunks = AsyncChunkedStreamBuf::DefaultMaxQueuedChunks) :
			std::ostream(&m_buf),
			m_buf(std::move(callback), chunkSize, maxQueuedChunks)
		{
		}

		explicit AsyncChunkedOStream(
			std::shared_ptr<std::ostream> dest,
			std::size_t chunkSize = AsyncChunkedStreamBuf::DefaultChunkSize,
			std::size_t maxQueuedChunks = AsyncChunkedStreamBuf::DefaultMaxQueuedChunks) :
			std::ostream(&m_buf),
			m_buf(std::move(dest), chunkSize, maxQueuedChunks)
		{
		}

		explicit AsyncChunkedOStream(
			std::ostream& dest,
			std::size_t chunkSize = AsyncChunkedStreamBuf::DefaultChunkSize,
			std::size_t maxQueuedChunks = AsyncChunkedStreamBuf::DefaultMaxQueuedChunks) :
			std::ostream(&m_buf),
			m_buf(dest, chunkSize, maxQueuedChunks)
		{
		}

		[[nodiscard]] std::size_t GetTotalBytes() const noexcept
		{
			return m_buf.GetTotalBytes();
		}
		[[nodiscard]] AsyncChunkedStreamBuf& GetBuffer() noexcept
		{
			return m_buf;
		}

	private:
		AsyncChunkedStreamBuf m_buf;
	};

} // namespace cct

#endif // CONCERTO_PKGGENERATOR_CHUNKEDSTREAMBUF_HPP
