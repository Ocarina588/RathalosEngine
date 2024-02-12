#pragma once

#include <vector>
#include <memory>
#include "Utils.hpp"
#include "Instance.hpp"
#include "Surface.hpp"

namespace re {

	class PhysicalDevice {
	private:
		struct queueFamily_s {
			uint32_t graphics = INVALID_UINT32;
			uint32_t compute = INVALID_UINT32;
			uint32_t present = INVALID_UINT32;
			uint32_t transfer = INVALID_UINT32;
		};

		struct swapChainSupportDetails_s {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};
	public:
		static std::vector<PhysicalDevice> enumerate(re::Instance& instance);

		PhysicalDevice(VkPhysicalDevice physicalDevice);
		PhysicalDevice(void) {};
		~PhysicalDevice(void) {};

		bool isSuitable(void);

		VkPhysicalDevice ptr = nullptr;
		VkPhysicalDeviceProperties properties{};
		VkPhysicalDeviceFeatures features{};
		std::vector<VkQueueFamilyProperties> queueFamilyProperties;

		queueFamily_s queueFamily;
		swapChainSupportDetails_s swapChainSupportDetails;
		void getQueueIndices(re::Instance& instance, re::Surface& surface);
		void getSwapChainSupportDetails(re::Surface& surface);
		VkSurfaceFormatKHR getFormat(void);
		VkPresentModeKHR getPresentMode(void);
	private:
	};

	class Device;

	class SwapChain {
	public:
		SwapChain(VkSwapchainCreateInfoKHR createInfo, re::Device &device);
		~SwapChain(void);

		VkSwapchainKHR ptr;
		VkFormat format{};
		VkExtent2D extent{};
		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
		re::Device& device;
	private:
	};

	typedef std::shared_ptr<SwapChain> swapChain_ptr;

	class Device {
	private:
		struct queueHandles_s {
			VkQueue graphics = nullptr;
			VkQueue compute = nullptr;
			VkQueue present = nullptr;
			VkQueue transfer = nullptr;
		};

	public:
		Device(re::Instance& instance, re::Surface &surface);
		~Device(void);

		void dumpInfo(void);

		VkDevice ptr = nullptr;
		PhysicalDevice physicalDevice;
		queueHandles_s queueHandles;
		swapChain_ptr swapChain = nullptr;

		void createSwapChain(void);

	private:
		void pickPhysicalDevice(void);
		std::vector<VkDeviceQueueCreateInfo> getDeviceQueueCreateInfos(re::PhysicalDevice& ref);
		VkExtent2D getSwapChainExtent(void);
		std::vector<char const*> getExtensions(void);
		void getQueueHandles(void);
		re::Instance& instance;
		re::Surface& surface;
		float queuePriority = 1.0f;
	};
}