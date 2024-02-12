#include "Device.hpp"
#include <set>
#include <map>
#include <algorithm>

re::Device::Device(re::Instance& instance, re::Surface &surface) : instance(instance), surface(surface)
{
	pickPhysicalDevice();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = getDeviceQueueCreateInfos(physicalDevice);
	std::vector<char const*> extensions = getExtensions();

	VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	VkPhysicalDeviceFeatures deviceFeatures{};

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.pEnabledFeatures = &deviceFeatures;

	if (vkCreateDevice(physicalDevice.ptr, &createInfo, nullptr, &ptr) != VK_SUCCESS)
		throw std::runtime_error("failed to create device");

	getQueueHandles();

	createSwapChain();

	dumpInfo();
}

re::Device::~Device(void)
{
	swapChain.reset();
	vkDestroyDevice(ptr, nullptr);
}

void re::Device::pickPhysicalDevice(void)
{
	std::vector<re::PhysicalDevice> physicalDevices = re::PhysicalDevice::enumerate(instance);
	re::PhysicalDevice* ptr = nullptr;

	for (int i = 0; i < physicalDevices.size(); i++) {
		physicalDevices[i].getQueueIndices(instance, surface);
		physicalDevices[i].getSwapChainSupportDetails(surface);
		if (physicalDevices[i].isSuitable()) {
			ptr = &physicalDevices[i];
			break;
		}
	}

	if (!ptr)
		throw std::runtime_error("didn't find a suitable GPU");

	physicalDevice = *ptr;
}

void re::Device::getQueueHandles(void)
{
	if (physicalDevice.queueFamily.graphics != INVALID_UINT32)
		vkGetDeviceQueue(ptr, physicalDevice.queueFamily.graphics, 0, &queueHandles.graphics);
	if (physicalDevice.queueFamily.compute != INVALID_UINT32)
		vkGetDeviceQueue(ptr, physicalDevice.queueFamily.compute, 0, &queueHandles.compute);
	if (physicalDevice.queueFamily.present != INVALID_UINT32)
		vkGetDeviceQueue(ptr, physicalDevice.queueFamily.present, 0, &queueHandles.present);
	if (physicalDevice.queueFamily.transfer != INVALID_UINT32)
		vkGetDeviceQueue(ptr, physicalDevice.queueFamily.transfer, 0, &queueHandles.transfer);
}

void re::PhysicalDevice::getQueueIndices(re::Instance& instance, re::Surface& surface)
{
	for (int i = 0; i < queueFamilyProperties.size(); i++) {
		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			queueFamily.graphics = i;
		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			queueFamily.compute = i;
		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
			queueFamily.transfer = i;
		VkBool32 present = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(ptr, i, surface.ptr, &present);
		if (present)
			queueFamily.present = i;
	}

}

std::vector<re::PhysicalDevice> re::PhysicalDevice::enumerate(re::Instance& instance)
{
	uint32_t count = 0;
	vkEnumeratePhysicalDevices(instance.ptr, &count, nullptr);

	std::vector<VkPhysicalDevice> physicalDevices(count);
	vkEnumeratePhysicalDevices(instance.ptr, &count, physicalDevices.data());
	std::vector<PhysicalDevice> vec(physicalDevices.data(), physicalDevices.data() + count);

	return vec;
}

re::PhysicalDevice::PhysicalDevice(VkPhysicalDevice physicalDevice) : ptr(physicalDevice)
{
	uint32_t count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(ptr, &count, nullptr);
	queueFamilyProperties.resize(count);
	vkGetPhysicalDeviceQueueFamilyProperties(ptr, &count, queueFamilyProperties.data());
	vkGetPhysicalDeviceProperties(ptr, &properties);
	vkGetPhysicalDeviceFeatures(ptr, &features);

}

bool re::PhysicalDevice::isSuitable(void)
{
	return (
		features.geometryShader &&
		swapChainSupportDetails.presentModes.empty() == false &&
		swapChainSupportDetails.formats.empty() == false &&
		queueFamily.present != INVALID_UINT32 &&
		queueFamily.graphics != INVALID_UINT32
	);
}

std::vector<char const*> re::Device::getExtensions(void)
{
	std::vector<char const*> extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	uint32_t count = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice.ptr, nullptr, &count, nullptr);
	std::vector<VkExtensionProperties> properties(count);
	vkEnumerateDeviceExtensionProperties(physicalDevice.ptr, nullptr, &count, properties.data());

	for (int i = 0; i < extensions.size(); i++) {
		bool found = false;
		for (int j = 0; j < properties.size(); j++)
			if (!std::strcmp(extensions[i], properties[j].extensionName))
				found = true;
		if (!found)
			throw std::runtime_error("Device extension nout found: " + std::string(extensions[i]));
	}

	return extensions;
}

std::vector<VkDeviceQueueCreateInfo> re::Device::getDeviceQueueCreateInfos(re::PhysicalDevice& ref)
{
	std::vector<VkDeviceQueueCreateInfo> createInfos;
	bool seen[32] = { false };

	uint32_t* families[4] = {
		&ref.queueFamily.graphics, 
		&ref.queueFamily.compute, 
		&ref.queueFamily.present, 
		&ref.queueFamily.transfer 
	};

	for (int i = 0; i < 4; i++) {
		if (*families[i] >= 32 || seen[*families[i]]) continue;
		seen[*families[i]] = true;
		createInfos.push_back({ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO });
		createInfos.rbegin()->queueFamilyIndex = *families[i];
		createInfos.rbegin()->queueCount = 1;
		createInfos.rbegin()->pQueuePriorities = &queuePriority;
	}

	return createInfos;
}

void re::PhysicalDevice::getSwapChainSupportDetails(re::Surface &surface)
{
	uint32_t count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(ptr, surface.ptr, &count, nullptr);
	swapChainSupportDetails.formats.resize(count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(ptr, surface.ptr, &count, swapChainSupportDetails.formats.data());

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ptr, surface.ptr, &swapChainSupportDetails.capabilities);
	vkGetPhysicalDeviceSurfacePresentModesKHR(ptr, surface.ptr, &count, nullptr);
	swapChainSupportDetails.presentModes.resize(count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(ptr, surface.ptr, &count, swapChainSupportDetails.presentModes.data());
}

VkSurfaceFormatKHR re::PhysicalDevice::getFormat(void)
{
	for (int i = 0; i < swapChainSupportDetails.formats.size(); i++)
		if (swapChainSupportDetails.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && swapChainSupportDetails.formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			return swapChainSupportDetails.formats[i];
	return swapChainSupportDetails.formats[0];
}

VkPresentModeKHR re::PhysicalDevice::getPresentMode(void)
{
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D re::Device::getSwapChainExtent(void)
{
	VkExtent2D extent = surface.getSurfaceSize();
	VkSurfaceCapabilitiesKHR& capabilities = physicalDevice.swapChainSupportDetails.capabilities;
	return { 
		std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
	};
}

void re::Device::createSwapChain(void)
{
	VkSwapchainCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	VkSurfaceCapabilitiesKHR capabilities = physicalDevice.swapChainSupportDetails.capabilities;
	VkSurfaceFormatKHR format = physicalDevice.getFormat();
	createInfo.surface = surface.ptr;
	createInfo.minImageCount = capabilities.minImageCount;
	createInfo.imageFormat = format.format;
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.imageExtent = getSwapChainExtent();
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	//only work on my computer :TODO
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = physicalDevice.getPresentMode();
	createInfo.clipped = VK_TRUE;

	swapChain = std::make_shared<SwapChain>(createInfo, *this);
}

void re::Device::dumpInfo(void)
{
	std::cout << TERMINAL_COLOR_MAGENTA << "choosen GPU: " << physicalDevice.properties.deviceName << TERMINAL_COLOR_RESET << std::endl;
	std::cout << TERMINAL_COLOR_YELLOW << "Queues available:" << TERMINAL_COLOR_RESET << std::endl;
	std::cout << TAB << (queueHandles.graphics ? TERMINAL_COLOR_GREEN : TERMINAL_COLOR_RED) << "graphics queue" << std::endl;
	std::cout << TAB << (queueHandles.compute ? TERMINAL_COLOR_GREEN : TERMINAL_COLOR_RED) << "compute queue" << std::endl;
	std::cout << TAB << (queueHandles.present ? TERMINAL_COLOR_GREEN : TERMINAL_COLOR_RED) << "present queue" << std::endl;
	std::cout << TAB << (queueHandles.transfer ? TERMINAL_COLOR_GREEN : TERMINAL_COLOR_RED) << "transfer queue" << std::endl;


	std::cout << TERMINAL_COLOR_YELLOW << "Present Modes available:" << TERMINAL_COLOR_GREEN << std::endl;

	std::vector<std::string> m = { "IMMEDIATE", "MAILBOX", "FIFO", "FIFO_RELAXED" };

	for (int i = 0; i < physicalDevice.swapChainSupportDetails.presentModes.size(); i++)
		if (physicalDevice.swapChainSupportDetails.presentModes[i] < 4)
			std::cout << TAB << m[physicalDevice.swapChainSupportDetails.presentModes[i]] << std::endl;

	std::cout << TERMINAL_COLOR_RESET << std::endl;

}

re::SwapChain::SwapChain(VkSwapchainCreateInfoKHR createInfo, re::Device &device) : device(device)
{
	if (vkCreateSwapchainKHR(device.ptr, &createInfo, nullptr, &ptr) != VK_SUCCESS)
		throw std::runtime_error("failed to created SwapChain");

	format = createInfo.imageFormat;
	extent = createInfo.imageExtent;

	uint32_t count = 0;
	vkGetSwapchainImagesKHR(device.ptr, ptr, &count, nullptr);
	images.resize(count);
	imageViews.resize(count);
	vkGetSwapchainImagesKHR(device.ptr, ptr, &count, images.data());

	for (int i = 0; i < count; i++) {
		VkImageViewCreateInfo info { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		info.image = images[i];
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = format;

		info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device.ptr, &info, nullptr, &imageViews[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to create ImageView");
	}
}

re::SwapChain::~SwapChain(void)
{
	for (int i = 0; i < imageViews.size(); i++)
		vkDestroyImageView(device.ptr, imageViews[i], nullptr);
	vkDestroySwapchainKHR(device.ptr, ptr, nullptr);
}