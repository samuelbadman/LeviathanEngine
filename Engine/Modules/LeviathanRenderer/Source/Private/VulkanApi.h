#pragma once

namespace LeviathanRenderer
{
	VkAllocationCallbacks* CreateVulkanAllocator();
	bool CreateVulkanInstance(unsigned int apiVersion, std::string_view applicationName, std::string_view engineName, const VkAllocationCallbacks* allocator, VkInstance& outInstance);
	void DestroyVulkanInstance(VkInstance instance, VkAllocationCallbacks* allocator);
}