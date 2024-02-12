#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Window.hpp"
#include "Instance.hpp"

namespace re {

	class Surface {
	public:
		Surface(re::Window& window, re::Instance& instance);
		~Surface(void);

		VkSurfaceKHR ptr;
		VkExtent2D getSurfaceSize(void);
	private:
		re::Instance& instance;
		re::Window& window;
	};
}