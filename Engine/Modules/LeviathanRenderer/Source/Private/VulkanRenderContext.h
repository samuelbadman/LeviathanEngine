#pragma once

namespace LeviathanRenderer
{
	class RenderContextInstance
	{
	private:
		VkSurfaceKHR VulkanSurface = VK_NULL_HANDLE;
		VkSwapchainKHR VulkanSwapchain = VK_NULL_HANDLE;
		std::vector<VkImage> VulkanSwapchainImages = {};
		std::vector<VkImageView> VulkanSwapchainImageViews = {};
		VkFormat SwapchainFormat = VK_FORMAT_UNDEFINED;
		VkExtent2D SwapchainExtent = {};

		// Render context settings. Must be applied with RenderContext::ApplyRenderContextSettings() when changing after context initialization.
		bool VSyncEnabled = false;
		unsigned int SwapchainImageCount = 0;

	public:
		bool Initialize(VkInstance instance,
			void* platformWindowHandle,
			VkAllocationCallbacks* const allocator,
			VkPhysicalDevice physicalDevice,
			VkColorSpaceKHR swapchainColorSpace,
			VkFormat swapchainFormat,
			VkDevice device);

		bool Shutdown(VkInstance instance, VkAllocationCallbacks* const allocator, VkDevice device);

		// Functions to set render context settings.
		void SetVSyncEnabled(const bool enabled) { VSyncEnabled = enabled; }
		void SetSwapchainImageCount(const unsigned int count) { SwapchainImageCount = count; }
	};
}