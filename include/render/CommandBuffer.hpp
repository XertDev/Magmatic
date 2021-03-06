#ifndef MAGMATIC_COMMANDBUFFER_HPP
#define MAGMATIC_COMMANDBUFFER_HPP

#include <vulkan/vulkan.hpp>
#include "Framebuffers.hpp"
#include "RenderPass.hpp"
#include "Pipeline.hpp"


namespace magmatic::render
{

	class CommandBuffer
	{
	public:
		explicit CommandBuffer(const CommandPool& pool, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer& operator=(const CommandBuffer&) = delete;

		CommandBuffer(CommandBuffer&& rhs) noexcept = default;
		CommandBuffer& operator=(CommandBuffer&& rhs) noexcept = default;

		~CommandBuffer() = default;

		const vk::UniqueCommandBuffer& beginRecording(const vk::CommandBufferUsageFlags& usage = vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		void endRecording();

		void submitWait();

		void submit(const Semaphores& imageAcquiredSemaphores, const Semaphores& renderFinishedSemaphores, const vk::UniqueFence& fence, size_t index) const;

		[[nodiscard]] static std::vector<CommandBuffer> createCommandBuffers(size_t count, const CommandPool& pool,
				vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
		[[nodiscard]] static std::vector<std::unique_ptr<CommandBuffer>> createCommandBuffersUnique(size_t count, const CommandPool& pool,
		                                                                     vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
		static void reCreateCommandBuffers(std::vector<CommandBuffer>& oldBuffers, const CommandPool& pool,
				vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
	private:
		vk::Queue queue;
		vk::UniqueCommandBuffer command_buffer;

		bool recording = false;
	};
}


#endif //MAGMATIC_COMMANDBUFFER_HPP
