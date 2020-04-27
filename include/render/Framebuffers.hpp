#ifndef MAGMATIC_FRAMEBUFFERS_HPP
#define MAGMATIC_FRAMEBUFFERS_HPP

#include <vulkan/vulkan.hpp>

namespace magmatic::render
{
	class Framebuffers
	{
		friend class LogicalDevice;

		explicit Framebuffers(std::vector<vk::UniqueFramebuffer>& framebuffers) : framebuffers(std::move(framebuffers)) {};
	public:
		const std::vector<vk::UniqueFramebuffer> framebuffers;

		Framebuffers(Framebuffers&) = delete;
		Framebuffers& operator=(Framebuffers&) = delete;

		[[nodiscard]] size_t getSize() const { return framebuffers.size(); }
	};
}


#endif //MAGMATIC_FRAMEBUFFERS_HPP