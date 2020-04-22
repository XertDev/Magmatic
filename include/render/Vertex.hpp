#ifndef MAGMATIC_VERTEX_HPP
#define MAGMATIC_VERTEX_HPP

#include <iostream>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace magmatic::render {
	struct Vertex {
		glm::vec2 position;

		static constexpr uint32_t binding = 0;

		static vk::VertexInputBindingDescription getBindingDescription() {
			return vk::VertexInputBindingDescription(
					binding,
					sizeof(Vertex),
					vk::VertexInputRate::eVertex
			);
		}

		static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
			return {
					vk::VertexInputAttributeDescription(0, binding, vk::Format::eR32G32Sfloat, offsetof(Vertex, position)),
			};
		}
	};
}

#endif //MAGMATIC_VERTEX_HPP
