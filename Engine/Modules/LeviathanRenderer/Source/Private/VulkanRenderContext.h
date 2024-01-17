#pragma once

namespace LeviathanRenderer
{
	namespace VulkanApi
	{
		struct VulkanPhysicalDeviceQueueFamilyIndices;
	}

	class RenderContextInstance
	{
	private:
		VkSurfaceKHR VulkanSurface = VK_NULL_HANDLE;
		VkSwapchainKHR VulkanSwapchain = VK_NULL_HANDLE;
		std::vector<VkImage> VulkanSwapchainImages = {};
		std::vector<VkImageView> VulkanSwapchainImageViews = {};
		std::vector<VkFramebuffer> VulkanSwapchainFramebuffers = {};

		VkFormat SwapchainFormat = VK_FORMAT_UNDEFINED;
		VkExtent2D SwapchainExtent = {};
		size_t CurrentFrame = 0;

		// Render context settings. Need to be set before calling Initialize. Must be applied with RenderContext::ApplyRenderContextSettings() when changing after context initialization.
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

		void IncrementCurrentFrame();

		// Functions to set render context settings.
		void SetVSyncEnabled(const bool enabled) { VSyncEnabled = enabled; }
		void SetSwapchainImageCount(const unsigned int count) { SwapchainImageCount = count; }
	};
}