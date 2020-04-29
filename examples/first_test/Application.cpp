#define GLM_FORCE_RADIANS
#include <algorithm>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>
#include "render/UniformBufferObject.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Application.hpp"
#include "render/Vertex.hpp"

//TODO: Check if Initializer List is in correct order
Application::Application(const std::string& mode):
vertices(std::move(getVertexConfig(mode))),
indices(std::move(getIndexConfig(mode))),
window(magmatic::render::Window(DEFAULT_NAME)),
instance(magmatic::render::Instance(DEFAULT_NAME, window.getRequiredExtensions())),
surface(instance.createSurface(window)),
physicalDevice(magmatic::render::PhysicalDevice(instance.getBestDevice())),
logicalDevice(physicalDevice, surface),
vertShader(logicalDevice.createShader("./examples/first_test/vert.spv", vk::ShaderStageFlagBits::eVertex)),
fragShader(logicalDevice.createShader("./examples/first_test/frag.spv", vk::ShaderStageFlagBits::eFragment)),
swapChain(logicalDevice.createSwapchain(surface, window.getSize().first, window.getSize().second)),
commandPool(logicalDevice.createCommandPool(magmatic::render::QueueType::GraphicalQueue)),
depthResources(logicalDevice.createDepthResources(swapChain.extent, commandPool)),
renderPass(logicalDevice.createRenderPass(surface, depthResources)),
descriptorSetLayout(logicalDevice.createDescriptorSetLayout(bindings)),
pipelineLayout(logicalDevice.createPipelineLayout(descriptorSetLayout)),
pipeline(logicalDevice.createPipeline<magmatic::render::Vertex>(swapChain.extent.width, swapChain.extent.height, {vertShader, fragShader}, renderPass, pipelineLayout)),
framebuffers(logicalDevice.createFramebuffers(renderPass, swapChain, depthResources.imageView.get())),
vertexBuffer(logicalDevice.createVertexBuffer(vertices, commandPool)),
indexBuffer(logicalDevice.createIndexBuffer(indices, commandPool)),
uniformBuffers(logicalDevice.createUniformBuffers(swapChain)),
commandBuffers(logicalDevice.createCommandBuffers(commandPool, framebuffers.size())),
descriptorSets(logicalDevice.createDescriptorSets(swapChain.images_.size(), descriptorSetLayout, descriptor_types)),
fences(logicalDevice.createFences(MAX_FRAMES_IN_FLIGHT)),
imageAcquiredSemaphores(logicalDevice.createSemaphores(magmatic::render::SemaphoreType::ImageAvailableSemaphore, MAX_FRAMES_IN_FLIGHT)),
renderFinishedSemaphores(logicalDevice.createSemaphores(magmatic::render::SemaphoreType::RenderFinishedSemaphore, MAX_FRAMES_IN_FLIGHT)),
texture(logicalDevice.createTexture(magmatic::render::Bitmap("examples/resources/statue.jpg"), commandPool)),
sampler(logicalDevice.createSampler())
{
	spdlog::info("Application constructor called and finished work");
}

void Application::recreateSwapChain() {
	spdlog::info("RecreatingSwapChain");
	auto size = window.getFramebufferSize();
	while(size.first == 0 || size.second == 0) {
		size = window.getFramebufferSize();
		glfwWaitEvents();
	}

	logicalDevice.waitIdle();

	//logicalDevice.device->destroySwapchainKHR(swapChain.swapchain_.get(), nullptr);

	swapChain = logicalDevice.createSwapchain(surface, window.getSize().first, window.getSize().second);
	renderPass = logicalDevice.createRenderPass(surface, depthResources);
	pipeline = logicalDevice.createPipeline<magmatic::render::Vertex>(swapChain.extent.width, swapChain.extent.height, {vertShader, fragShader}, renderPass, pipelineLayout);
	depthResources = logicalDevice.createDepthResources(swapChain.extent, commandPool);
	framebuffers = logicalDevice.createFramebuffers(renderPass, swapChain, depthResources.imageView.get());
	descriptorSets = logicalDevice.createDescriptorSets(swapChain.images_.size(), descriptorSetLayout, descriptor_types);
	commandBuffers = logicalDevice.createCommandBuffers(commandPool, framebuffers.size());
}

void Application::run() {
	for(size_t i = 0; i < uniformBuffers.size(); ++i)
	{
		std::vector<magmatic::render::DescriptorWriteUpdate> updates;
		magmatic::render::DescriptorWriteUpdate write_update;
		write_update.type = magmatic::render::DescriptorWriteUpdate::eUniform;
		write_update.dst_binding = 0;
		write_update.dst_array_elem = 0;
		vk::DescriptorBufferInfo info(
				uniformBuffers[i].buffer.get(),
				0,
				sizeof(magmatic::render::UniformBufferObject)
				);
		write_update.data_info = info;
		updates.emplace_back(write_update);

		updates.emplace_back(texture.getWriteInfo(1, 0));
		updates.emplace_back(sampler.getWriteInfo(2, 0));

		logicalDevice.updateDescriptorSet(descriptorSets.sets[i], updates);
	}


	currentFrame = 0;
	imagesInFlight.resize(swapChain.images_.size(), -1);

	vk::Buffer vertexBuffers[] = { vertexBuffer.buffer.get() };
	vk::DeviceSize offsets[] = { 0 };

	for(size_t i = 0; i < commandBuffers.size(); i++) {
		auto& commandBufferHandle = std::move(commandBuffers[i].beginRecording(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
		std::array<vk::ClearValue, 2> clearValues;
		clearValues[0].color =  vk::ClearColorValue(std::array<float, 4>({0.2f, 0.2f, 0.2f, 1.0f}));
		clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
		vk::RenderPassBeginInfo beginInfo(renderPass.renderPass.get(),
		                                  framebuffers.framebuffers[i].get(),
		                                  vk::Rect2D(vk::Offset2D(0, 0), swapChain.extent),
		                                  static_cast<uint32_t>(clearValues.size()),
		                                  clearValues.data());
		commandBufferHandle->beginRenderPass(beginInfo, vk::SubpassContents::eInline);
		commandBufferHandle->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.pipeline.get());
		commandBufferHandle->bindVertexBuffers(0, 1, vertexBuffers, offsets);
		commandBufferHandle->bindIndexBuffer(indexBuffer.buffer.get(), 0, vk::IndexType::eUint32);
		commandBufferHandle->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout.get(), 0, 1, &descriptorSets.sets[i], 0, nullptr);
		commandBufferHandle->drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
		commandBufferHandle->endRenderPass();
		commandBuffers[i].endRecording();
	}

	while(!window.shouldClose()) {
		glfwPollEvents();
		drawFrame();
	}

	logicalDevice.waitIdle();
}

void Application::updateUniformBuffer(uint32_t currentBuffer) {
	static auto startTime = std::chrono::high_resolution_clock::now();
	magmatic::render::UniformBufferObject ubo = {};
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), swapChain.extent.width/(float) swapChain.extent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	logicalDevice.copyBufferMemory(uniformBuffers[currentBuffer].memory, sizeof(ubo), ubo);
}

void Application::drawFrame() {
	logicalDevice.waitForFences(fences[currentFrame], fenceTimeout);
	uint32_t currentBuffer;
	auto state = logicalDevice.acquireNextImageKHR(swapChain, imageAcquiredSemaphores, currentFrame, currentBuffer, 0);
	if(state == vk::Result::eNotReady)
	{
		std::this_thread::yield();
		return;
	} else if(state == vk::Result::eErrorOutOfDateKHR) {
		recreateSwapChain();
		// TODO: check if needed
		//  run();
		return;
	} else if(state != vk::Result::eSuccess && state != vk::Result::eSuboptimalKHR) {
		spdlog::error("Failed to acquire swap chain image!");
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	if(imagesInFlight[currentBuffer] != -1) {
		logicalDevice.waitForFences(fences[imagesInFlight[currentBuffer]], fenceTimeout);
	}
	imagesInFlight[currentBuffer] = currentFrame;
	logicalDevice.resetFences(fences[currentFrame]);
	updateUniformBuffer(currentBuffer);
	logicalDevice.submitToGraphicsQueue(imageAcquiredSemaphores, renderFinishedSemaphores, commandBuffers[currentBuffer], fences[currentFrame], currentFrame);
	try {
		state = logicalDevice.presentKHR(renderFinishedSemaphores, currentFrame, swapChain, currentBuffer);
	} catch(vk::OutOfDateKHRError&) {
		recreateSwapChain();
		// TODO: check if needed
		//  run();
	}
	if(state == vk::Result::eSuboptimalKHR) {
		recreateSwapChain();
		// TODO: check if needed
		//  run();
		return;
	} else if(state != vk::Result::eSuccess) {
		spdlog::error("Failed to present swap chain image!");
		throw std::runtime_error("Failed to present swap chain image!");
	}

	currentFrame = (currentFrame+1)%MAX_FRAMES_IN_FLIGHT;
}

std::vector<magmatic::render::Vertex> Application::getVertexConfig(const std::string& mode) const {
	if(mode == "square") {
		return squareVertices;
	} else if(mode == "triangle") {
		return triangleVertices;
	} else if(mode == "hourglass") {
		return hourglassVertices;
	} else if(mode == "hourglass on square") {
		std::vector<magmatic::render::Vertex> res;
		res.reserve(hourglassVertices.size() + squareVertices.size());
		res.insert(res.end(), squareVertices.begin(), squareVertices.end());
		res.insert(res.end(), hourglassVertices.begin(), hourglassVertices.end());
		return res;
	} else {
		spdlog::error("Mode {} not implemented", mode);
		throw std::runtime_error("Mode '" + mode + "' is not yet implemented");
	}
}

std::vector<uint32_t> Application::getIndexConfig(const std::string& mode) const {
	if(mode == "square") {
		return squareIndices;
	} else if(mode == "triangle") {
		return triangleIndices;
	} else if(mode == "hourglass") {
		return hourglassIndices;
	} else if(mode == "hourglass on square") {
		std::vector<uint32_t> res;
		res.reserve(hourglassIndices.size() + squareIndices.size());
		res.insert(res.end(), squareIndices.begin(),squareIndices.end());
		for(uint32_t index : hourglassIndices) {
			res.emplace_back(index + static_cast<uint32_t>(squareVertices.size()));
		}
		return res;
	} else {
		spdlog::error("Mode {} not implemented", mode);
		throw std::runtime_error("Mode '" + mode + "' is not yet implemented");
	}
}