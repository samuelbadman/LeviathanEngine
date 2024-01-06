#pragma once

namespace LeviathanRenderer
{
	class RenderContextInstance
	{
	private:
		VkSurfaceKHR VulkanSurface = VK_NULL_HANDLE;

	public:
		VkSurfaceKHR& GetVulkanSurfaceHandleReference() { return VulkanSurface; }
		VkSurfaceKHR GetVulkanSurface() const { return VulkanSurface; }
	};
}