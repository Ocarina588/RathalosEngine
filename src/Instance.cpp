#pragma once

#include "Instance.hpp"

re::Instance::Instance(void)
{
	std::vector<char const*> extensions = getExtensions();
	std::vector<char const*> layers = getLayers();

	VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
	createInfo.ppEnabledLayerNames = layers.data();
	
	if (debug) {
		VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = getDebugMessengerCreateInfo();
		createInfo.pNext = &messengerCreateInfo;
	}

	if (vkCreateInstance(&createInfo, nullptr, &ptr) != VK_SUCCESS)
		throw std::runtime_error("failed to create Instance");

	if (debug)
		createDebugMessenger();
}

re::Instance::~Instance(void)
{
	destroyDebugMessenger();
	vkDestroyInstance(ptr, nullptr);
}

std::vector<char const*> re::Instance::getExtensions(void)
{
	uint32_t count = 0;
	char const ** ptr = glfwGetRequiredInstanceExtensions(&count);
	std::vector<char const*> extensions(ptr, ptr + count);

	if (debug)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
	std::vector<VkExtensionProperties> properties(count);
	vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data());

	for (int i = 0; i < extensions.size(); i++) {
		bool found = false;
		for (int j = 0; j < properties.size(); j++)
			if (!std::strcmp(extensions[i], properties[j].extensionName))
				found = true;
		if (!found)
			throw std::runtime_error("extension not found: " + std::string(extensions[i]));
	}

	return (extensions);
}

std::vector<char const*> re::Instance::getLayers(void)
{
	std::vector<char const*> layers = { "VK_LAYER_KHRONOS_validation" };

	uint32_t count = 0;
	vkEnumerateInstanceLayerProperties(&count, nullptr);

	std::vector<VkLayerProperties> properties(count);
	vkEnumerateInstanceLayerProperties(&count, properties.data());
		
	for (int i = 0; i < layers.size(); i++) {
		bool found = false;
		for (int j = 0; j < properties.size(); j++)
			if (!std::strcmp(layers[i], properties[j].layerName))
				found = true;
		if (!found)
			throw std::runtime_error("layer not found: " + std::string(layers[i]));
	}

	return (layers);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		return VK_FALSE;

	std::cout << TERMINAL_COLOR_CYAN << "validation layer: ";

	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: std::cout << TERMINAL_COLOR_BLUE;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: std::cout << TERMINAL_COLOR_GREEN;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: std::cout << TERMINAL_COLOR_YELLOW;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: std::cout << TERMINAL_COLOR_RED;
		break;
	}

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	printf(TERMINAL_COLOR_RESET);

	return VK_FALSE;
}

void re::Instance::createDebugMessenger(void)
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = getDebugMessengerCreateInfo();
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(ptr, "vkCreateDebugUtilsMessengerEXT");

	if (!func)
		throw std::runtime_error("failed to get function");
	if (func(ptr, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
		throw std::runtime_error("failed to create debugMessenger");
}

void re::Instance::destroyDebugMessenger(void)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(ptr, "vkDestroyDebugUtilsMessengerEXT");

	if (!func)
		throw std::runtime_error("failed to get function");

	func(ptr, debugMessenger, nullptr);
}

VkDebugUtilsMessengerCreateInfoEXT re::Instance::getDebugMessengerCreateInfo(void)
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	return (createInfo);
}