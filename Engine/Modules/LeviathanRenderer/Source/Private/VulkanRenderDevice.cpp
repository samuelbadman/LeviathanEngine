#include "RenderDevice.h"
#include "VulkanApi.h"
#include "Logging.h"

namespace LeviathanRenderer
{
	namespace RenderDevice
	{
		static VkAllocationCallbacks* VulkanAllocator = nullptr;
		static VkInstance VulkanInstance = VK_NULL_HANDLE;
		static VkPhysicalDeviceProperties VulkanPhysicalDeviceProperties = {};
		static VkPhysicalDeviceMemoryProperties VulkanPhysicalDeviceMemoryProperties = {};
		static VkPhysicalDeviceFeatures VulkanPhysicalDeviceFeatures = {};
		static VkPhysicalDevice VulkanPhysicalDevice = {};
		static VulkanPhysicalDeviceQueueFamilyIndices PhysicalDeviceQueueFamilyIndices = {};

		bool Initialize()
		{
			VulkanAllocator = CreateVulkanAllocator();

			if (!CreateVulkanInstance(VK_API_VERSION_1_3, "LeviathanApplication", "LeviathanEngine", VulkanAllocator, VulkanInstance))
			{
				return false;
			}

			if (!SelectPhysicalDevice(VulkanInstance, VulkanPhysicalDeviceProperties, VulkanPhysicalDeviceMemoryProperties, VulkanPhysicalDeviceFeatures, VulkanPhysicalDevice))
			{
				return false;
			}

			unsigned long long availableVideoMemoryGb = 0;
			if (!GetPhysicalDeviceVideoMemorySizeGb(VulkanPhysicalDeviceMemoryProperties, availableVideoMemoryGb))
			{
				return false;
			}

			LEVIATHAN_LOG("Vulkan render device: Selected physical device: Name %s, Dedicated video memory %zu GB", VulkanPhysicalDeviceProperties.deviceName, availableVideoMemoryGb);

			if (!GetPhysicalDeviceQueueFamilyIndices(VulkanInstance, VulkanPhysicalDevice, PhysicalDeviceQueueFamilyIndices))
			{
				return false;
			}



			return true;
		}

		bool Shutdown()
		{
			DestroyVulkanInstance(VulkanInstance, VulkanAllocator);

			return true;
		}
	}
}