#include "RenderDevice.h"
#include "VulkanHelpers.h"

namespace LeviathanRenderer
{
	namespace RenderDevice
	{
		static VkAllocationCallbacks* VulkanAllocator = nullptr;
		static VkInstance VulkanInstance = VK_NULL_HANDLE;

		bool Initialize()
		{
			VulkanAllocator = CreateVulkanAllocator();

			if (!CreateVulkanInstance(VK_API_VERSION_1_3, "LeviathanApplication", "LeviathanEngine", VulkanAllocator, VulkanInstance))
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