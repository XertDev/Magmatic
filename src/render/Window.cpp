#include "render/Window.hpp"
#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vulkan/vulkan.hpp>

namespace {
	void errorCallback(int error, const char* description) {
		spdlog::error("GLFW error: {} Error code: {}", description, error);
	}
}

magmatic::render::Window::Window(const std::string& name, int width, int height)
{
	glfwSetErrorCallback(errorCallback);

	if(glfwInit() == GLFW_FALSE) {
		throw std::runtime_error("GLFW: Failed to initialize");
	}

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	const auto temp_handler = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

	if(temp_handler == nullptr)
	{
		glfwTerminate();
		throw std::runtime_error("GLFW: Failed to create window");
	}
	window = std::unique_ptr<GLFWwindow, GLFWWindowDeleter> (temp_handler, GLFWWindowDeleter());
}

void magmatic::render::Window::setName(const std::string& new_name) noexcept
{
	window_name = new_name;
	glfwSetWindowTitle(window.get(), new_name.c_str());
}

void magmatic::render::Window::setSize(int width, int height)
{
	if(width <=0 || height <= 0)
	{
		throw std::runtime_error("Failed to resize window. Invalid size!");
	}

	glfwSetWindowSize(window.get(), width, height);
}

void magmatic::render::Window::hide() noexcept
{
	glfwHideWindow(window.get());
}

void magmatic::render::Window::minimize() noexcept
{
	glfwIconifyWindow(window.get());
}

void magmatic::render::Window::show() noexcept
{
	glfwShowWindow(window.get());
}

void magmatic::render::Window::maximize() noexcept
{
	glfwMaximizeWindow(window.get());
}

void magmatic::render::Window::restore() noexcept
{
	glfwRestoreWindow(window.get());
}

void magmatic::render::Window::focus() noexcept
{
	glfwFocusWindow(window.get());
}

std::pair<int, int> magmatic::render::Window::getSize() const
{
	int width;
	int height;
	glfwGetWindowSize(window.get(), &width, &height);

	return std::make_pair(width, height);
}

std::string magmatic::render::Window::getName() const noexcept
{
	return window_name;
}

vk::Extent2D magmatic::render::Window::getFramebufferSize() const {
	int width;
	int height;
	glfwGetFramebufferSize(window.get(), &width, &height);
	vk::Extent2D res = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
	};
	return res;
}

std::vector<std::string> magmatic::render::Window::getRequiredExtensions() const
{
	uint32_t glfw_extension_count = 0;
	const char ** glfw_extensions;

	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	std::vector<std::string> extensions;
	extensions.reserve(glfw_extension_count);
	std::copy_n(glfw_extensions, glfw_extension_count, std::back_inserter(extensions));

	return extensions;
}

const std::unique_ptr<GLFWwindow, magmatic::render::Window::GLFWWindowDeleter> &magmatic::render::Window::getWindow() const
{
	return this->window;
}

void magmatic::render::Window::setCursor(const Cursor &cursor) const noexcept
{
	glfwSetCursor(window.get(), cursor.cursor_);
}

void magmatic::render::Window::setMonitor(const Monitor &monitor, const Monitor::VideoMode& mode) const noexcept
{
	glfwSetWindowMonitor(window.get(), monitor.monitor_handle_, 0, 0, mode.width, mode.height, mode.refreshRate);
}

void magmatic::render::Window::setAspectRatio(int numer, int denom) const noexcept
{
	glfwSetWindowAspectRatio(window.get(), numer, denom);
}

void magmatic::render::Window::setSizeConstraints(const std::pair<int, int>& min, const std::pair<int, int>& max) const noexcept
{
	const int& min_width = std::get<0>(min);
	const int& min_height = std::get<1>(min);

	const int& max_width = std::get<0>(max);
	const int& max_height = std::get<1>(max);

	glfwSetWindowSizeLimits(window.get(), min_width, min_height, max_width, max_height);
}

void magmatic::render::Window::GLFWWindowDeleter::operator()(GLFWwindow* pointer) noexcept
{
	glfwDestroyWindow(pointer);
}


