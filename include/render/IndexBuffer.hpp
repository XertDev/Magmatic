#ifndef MAGMATIC_INDEXBUFFER_HPP
#define MAGMATIC_INDEXBUFFER_HPP

#include "Buffer.hpp"
#include "LogicalDevice.hpp"
#include "CommandPool.hpp"
#include <vulkan/vulkan.hpp>
#include <vector>

namespace magmatic::render
{
	class IndexBuffer : public Buffer
	{
	public:
		IndexBuffer(
				const LogicalDevice& l_device,
				const std::vector<uint32_t>& indices,
				const CommandPool& pool
		);

		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;

		IndexBuffer(IndexBuffer&& rhs) noexcept = default;
		IndexBuffer& operator=(IndexBuffer&& rhs) noexcept = default;

		~IndexBuffer() = default;

		[[nodiscard]] size_t indicesCount() const noexcept;

	private:
		size_t indices_count;
		constexpr static vk::BufferUsageFlags buffer_usage_flags =
				vk::BufferUsageFlagBits::eTransferDst
				| vk::BufferUsageFlagBits::eIndexBuffer;

		constexpr static vk::MemoryPropertyFlags memory_property = vk::MemoryPropertyFlagBits::eDeviceLocal;

	};
}


#endif //MAGMATIC_INDEXBUFFER_HPP
