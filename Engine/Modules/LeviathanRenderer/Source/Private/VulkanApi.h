#pragma once

namespace LeviathanRenderer
{
	struct VulkanPhysicalDeviceQueueFamilyIndices
	{
		std::optional<unsigned int> Graphics = {};
		std::optional<unsigned int> Compute = {};
		std::optional<unsigned int> Transfer = {};
		std::optional<unsigned int> SparseBinding = {};

		// Returns true if all queue family indices have been assigned otherwise, will return false.
		bool Complete() const;
	};

	struct VulkanDeviceQueueCountAndPriorities
	{
		unsigned int QueueCount = 0;

		// Each priority is assigned to each queue in the count. For example, queue 0 will be assigned QueuePriorities[0], queue 1 will be assigned QueuePriorities[1] etc...
		std::vector<float> QueuePriorities = {};
	};

	VkAllocationCallbacks* CreateVulkanAllocator();

	bool CreateVulkanInstance(unsigned int apiVersion, 
		std::string_view applicationName, 
		std::string_view engineName,
		const VkAllocationCallbacks* allocator, 
		VkInstance& outInstance);

	void DestroyVulkanInstance(VkInstance instance, const VkAllocationCallbacks* allocator);

	bool SelectVulkanPhysicalDevice(VkInstance instance, 
		VkPhysicalDeviceProperties& outPhysicalDeviceProperties,
		VkPhysicalDeviceMemoryProperties& outPhysicalDeviceMemoryProperties,
		VkPhysicalDeviceFeatures& outPhysicalDeviceFeatures,
		VkPhysicalDevice& outPhysicalDevice);

	// Returns the size of the physical device's video memory (vram) in gigabytes.
	bool GetVulkanPhysicalDeviceVideoMemorySizeGb(const VkPhysicalDeviceMemoryProperties& memoryProperties, unsigned long long& outVideoMemorySizeGb);

	bool GetVulkanPhysicalDeviceQueueFamilyIndices(VkPhysicalDevice const physicalDevice, VulkanPhysicalDeviceQueueFamilyIndices& outQueueFamilyIndices);

	bool IsPresentationSupportedOnQueue(VkInstance instance, VkPhysicalDevice physicalDevice, const unsigned int queueFamilyIndex);

	bool CreateVulkanLogicalDevice(VkPhysicalDevice const physicalDevice,
		const VulkanPhysicalDeviceQueueFamilyIndices& queueFamilyIndices,
		const VulkanDeviceQueueCountAndPriorities& graphicsQueueCountAndPriorities,
		const VulkanDeviceQueueCountAndPriorities& computeQueueCountAndPriorities,
		const VulkanDeviceQueueCountAndPriorities& transferQueueCountAndPriorities,
		const VulkanDeviceQueueCountAndPriorities& sparseBindingQueueCountAndPriorities,
		const VkAllocationCallbacks* const allocator,
		VkDevice& outDevice);

	void GetVulkanDeviceQueue(VkDevice device, const unsigned int queueFamilyIndex, const unsigned int queueIndex, VkQueue& outQueue);

	void DestroyVulkanLogicalDevice(VkDevice device, const VkAllocationCallbacks* allocator);

	bool CreateVulkanSurface(VkInstance const instance, void* const platformWindowHandle, VkAllocationCallbacks* const allocator, VkSurfaceKHR& outSurface);

	void DestroyVulkanSurface(VkInstance const instance, VkSurfaceKHR const surface, VkAllocationCallbacks* const allocator);
}