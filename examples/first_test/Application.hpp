#ifndef MAGMATIC_APPLICATION_H
#define MAGMATIC_APPLICATION_H

#include <render/Texture.hpp>
#include <render/Sampler.hpp>
#include <render/DescriptorSets.hpp>
#include <render/Fences.hpp>
#include <render/VertexBuffer.hpp>
#include <render/IndexBuffer.hpp>
#include <render/Shader.hpp>
#include <render/DepthResources.hpp>
#include <render/RenderPass.hpp>
#include <render/Pipeline.hpp>
#include <render/Framebuffers.hpp>
#include <render/CommandBuffer.hpp>
#include <render/UniformBuffer.hpp>
#include "render/Vertex.hpp"
#include "render/Window.hpp"
#include "render/Instance.hpp"
#include "render/Surface.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/LogicalDevice.hpp"
#include "render/SwapChain.hpp"
#include "render/Buffer.hpp"
#include "render/UniformBufferObject.hpp"

class Application {
    static constexpr const char* DEFAULT_NAME{"Test application"};
    /** Triangle */
	const std::vector<magmatic::render::Vertex> triangleVertices = {
			{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	};
	const std::vector<uint32_t> triangleIndices = {
			0, 1, 2
	};
	/** Square */
	const std::vector<magmatic::render::Vertex> squareVertices = {
			{{-0.8f, -0.8f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.8f, -0.8f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{-0.8f, 0.8f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{0.8f, 0.8f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

			{{-0.8f, -0.8f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.8f, -0.8f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{-0.8f, 0.8f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{0.8f, 0.8f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};
	const std::vector<uint32_t> squareIndices = {
			0, 1, 2, 1, 3, 2,
			4, 5, 6, 5, 7, 6,
	};
	/** Hourglass */
	const std::vector<magmatic::render::Vertex> hourglassVertices = {
			{{-0.5f, -0.5f, -0.8f}, {0.8f, 0.8f, 1.0f}, {0.0f, 0.0f}},
			{{-0.5f, 0.5f, -0.8f}, {0.8f, 0.8f, 1.0f}, {1.0f, 0.0f}},
			{{0.5f, 0.5f, -0.8f}, {0.8f, 0.8f, 1.0f}, {0.0f, 1.0f}},
			{{0.5f, -0.5f, -0.8f}, {0.8f, 0.8f, 1.0f}, {1.0f, 1.0f}},

			{{0.0f, 0.0f, 0.0f}, {0.8f, 0.8f, 1.0f}, {0.0f, 0.0f}},

			{{-0.5f, -0.5f, 0.8f}, {0.8f, 0.8f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.8f}, {0.8f, 0.8f, 1.0f}, {0.0f, 1.0f}},
			{{0.5f, 0.5f, 0.8f}, {0.8f, 0.8f, 1.0f}, {0.0f, 0.0f}},
			{{0.5f, -0.5f, 0.8f}, {0.8f, 0.8f, 1.0f}, {1.0f, 0.0f}},
	};
	const std::vector<uint32_t> hourglassIndices = {
			0, 1, 2, 2, 3, 0,// lower base
			1, 0, 4, 2, 1, 4, 3, 2, 4, 0, 3, 4,// Lower faces
			5, 6, 4, 6, 7, 4, 7, 8, 4, 8, 5, 4,// Upper faces
			7, 6, 5, 5, 8, 7// Upper base
	};

	const std::vector<vk::DescriptorSetLayoutBinding> bindings = {
			{
			0,
			vk::DescriptorType::eUniformBuffer,
			1,
			vk::ShaderStageFlagBits::eVertex,
			nullptr
			},
			{
				1,
				vk::DescriptorType::eSampledImage,
				1,
				vk::ShaderStageFlagBits::eFragment,
				nullptr
			},
			{
				2,
				vk::DescriptorType::eSampler,
				1,
				vk::ShaderStageFlagBits::eFragment,
				nullptr
			}
	};

	const std::vector<vk::DescriptorType> descriptor_types = {
			vk::DescriptorType::eUniformBuffer,
			vk::DescriptorType::eSampledImage,
			vk::DescriptorType::eSampler
	};

	const std::vector<magmatic::render::Vertex> vertices;
	const std::vector<uint32_t> indices;

    const magmatic::render::Window window;
    const magmatic::render::Instance instance;
    const magmatic::render::Surface surface;
	const magmatic::render::PhysicalDevice physicalDevice;
	const magmatic::render::LogicalDevice logicalDevice;
	magmatic::render::Shader vertShader;
	magmatic::render::Shader fragShader;
	const magmatic::render::SwapChain swapChain;
	const magmatic::render::CommandPool commandPool;
	const magmatic::render::DepthResources depthResources;
	const magmatic::render::RenderPass renderPass;
	const magmatic::render::DescriptorSets descriptorSets;
	const magmatic::render::Pipeline pipeline;
	const magmatic::render::Framebuffers framebuffers;
	const magmatic::render::VertexBuffer vertexBuffer;
	const magmatic::render::IndexBuffer indexBuffer;
	std::vector<magmatic::render::UniformBuffer<magmatic::render::UniformBufferObject>> uniformBuffers;
	std::vector<magmatic::render::CommandBuffer> commandBuffers;
	const magmatic::render::Fences fences;
	const magmatic::render::Semaphores imageAcquiredSemaphores;
	const magmatic::render::Semaphores renderFinishedSemaphores;
	const magmatic::render::Texture texture;
	const magmatic::render::Sampler sampler;
	std::vector<int> imagesInFlight;
public:
    explicit Application(const std::string& mode);

    void run();

private:
	static constexpr uint64_t fenceTimeout = 1000000;
	static constexpr size_t MAX_FRAMES_IN_FLIGHT = 3;

	size_t currentFrame = 0;
	uint32_t currentBuffer = 0;

	void updateUniformBuffer(uint32_t currentBuffer);

	[[nodiscard]] std::vector<magmatic::render::Vertex> getVertexConfig(const std::string &mode) const;

	[[nodiscard]] std::vector<uint32_t> getIndexConfig(const std::string& mode) const;
	void updateDescriptorSet();
	void recordCommandBuffer();
	void drawFrame();
};


#endif //MAGMATIC_APPLICATION_H
