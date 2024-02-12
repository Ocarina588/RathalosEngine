#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <stdexcept>

#include "Utils.hpp"

namespace re {

	class Instance {
	public:
		Instance(void);
		~Instance(void);

		VkInstance ptr;

	private:
		std::vector<char const *> getLayers(void);
		std::vector<char const *> getExtensions(void);
		
		void createDebugMessenger(void);
		void destroyDebugMessenger(void);

		VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo(void);

		bool const debug = true;
		VkDebugUtilsMessengerEXT debugMessenger;
	};
}