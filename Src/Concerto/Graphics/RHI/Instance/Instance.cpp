//
// Created by arthur on 12/05/2024.
//

#include "Concerto/Graphics/RHI/Instance/Instance.hpp"

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/RHI/Instance/APIImpl.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHI/VkRHI.hpp"

#ifdef CCT_PLATFORM_WINDOWS
#include "Concerto/Graphics/RHI/Dx12/Dx12RHI/Dx12RHI.hpp"
#endif

namespace cct::gfx::rhi
{
	Instance::Instance(Backend backend, ValidationLevel validationLevel) :
		m_backend(backend)
	{
		CCT_PROFILER_SCOPE();
		switch (backend)
		{
			case Backend::Vulkan:
			{
				if (Logger::GetContext())
					VkRHI::SetLogger(*Logger::GetContext());
				m_apiImpl = std::make_unique<VkRHI>();
				break;
			}
#ifdef CCT_PLATFORM_WINDOWS
			case Backend::DirectX12:
			{
				if (Logger::GetContext())
					Dx12RHI::SetLogger(*Logger::GetContext());
				m_apiImpl = std::make_unique<Dx12RHI>();
				break;
			}
#endif
		}
		m_apiImpl->Create(validationLevel);
	}

	std::span<const DeviceInfo> Instance::EnumerateDevices() const
	{
		CCT_ASSERT(m_apiImpl, "ConcertoGraphics: Tying to get an invalid ApiImpl");
		return m_apiImpl->EnumerateDevices();
	}

	std::unique_ptr<Device> Instance::CreateDevice(std::size_t index) const
	{
		CCT_ASSERT(m_apiImpl, "ConcertoGraphics: Tying to get an invalid ApiImpl");
		return m_apiImpl->CreateDevice(index);
	}

	APIImpl* Instance::GetImpl() const
	{
		CCT_ASSERT(m_apiImpl, "ConcertoGraphics: Tying to get an invalid ApiImpl");
		return m_apiImpl.get();
	}

	void Instance::SetLogger(Logger& logger)
	{
		Logger::SetContext(&logger);
	}
} // namespace cct::gfx::rhi
