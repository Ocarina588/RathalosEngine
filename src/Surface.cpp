#include "Surface.hpp"

re::Surface::Surface(re::Window& window, re::Instance& instance) : instance(instance), window(window)
{
	if (glfwCreateWindowSurface(instance.ptr, window.ptr, nullptr, &ptr) != VK_SUCCESS)
		throw std::runtime_error("failed to create surface");
}

re::Surface::~Surface(void)
{
	vkDestroySurfaceKHR(instance.ptr, ptr, nullptr);
}

VkExtent2D re::Surface::getSurfaceSize(void)
{
	int w, h;
	glfwGetFramebufferSize(window.ptr, &w, &h);
	return { static_cast<uint32_t>(w), static_cast<uint32_t>(h) };
}