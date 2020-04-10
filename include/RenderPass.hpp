#ifndef MAGMATIC_RENDERPASS_HPP
#define MAGMATIC_RENDERPASS_HPP

#include <vulkan/vulkan.hpp>

namespace magmatic {
    class RenderPass {
    public:
        const vk::UniqueRenderPass renderPass;

        explicit RenderPass(vk::UniqueRenderPass renderPass);

        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;
    };
}

#endif //MAGMATIC_RENDERPASS_HPP
