#pragma once

namespace LeviathanRenderer
{
	struct VulkanPhysicalDeviceQueueFamilyIndices
	{
		std::optional<unsigned int> Graphics = {};
		std::optional<unsigned int> Present = {};
		std::optional<unsigned int> Compute = {};
		std::optional<unsigned int> Transfer = {};
		std::optional<unsigned int> SparseBinding = {};

		// Returns true if all queue family indices have been assigned otherwise, will return false.
		bool Complete() const;
	};

	VkAllocationCallbacks* CreateVulkanAllocator();

	bool CreateVulkanInstance(unsigned int apiVersion, 
		std::string_view applicationName, 
		std::string_view engineName,
		const VkAllocationCallbacks* allocator, 
		VkInstance& outInstance);

	void DestroyVulkanInstance(VkInstance instance, VkAllocationCallbacks* allocator);

	bool SelectPhysicalDevice(VkInstance instance, 
		VkPhysicalDeviceProperties& outPhysicalDeviceProperties,
		VkPhysicalDeviceMemoryProperties& outPhysicalDeviceMemoryProperties,
		VkPhysicalDeviceFeatures& outPhysicalDeviceFeatures,
		VkPhysicalDevice& outPhysicalDevice);

	// Returns the size of the physical device's video memory (vram) in gigabytes.
	bool GetPhysicalDeviceVideoMemorySizeGb(const VkPhysicalDeviceMemoryProperties& memoryProperties, unsigned long long& outVideoMemorySizeGb);

	bool GetPhysicalDeviceQueueFamilyIndices(VkInstance const instance, VkPhysicalDevice const physicalDevice, VulkanPhysicalDeviceQueueFamilyIndices& outQueueFamilyIndices);
}