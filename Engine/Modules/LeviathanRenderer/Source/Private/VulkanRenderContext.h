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
		std::vector<VkFramebuffer> VulkanSwapchainFramebuffers = {};

		// In flight frame resources.
		std::vector<VkCommandBuffer> GraphicsVulkanCommandBuffers = {};
		std::vector<VkSemaphore> ImageAvailableVulkanSemaphores = {};
		std::vector<VkSemaphore> RenderFinishedVulkanSemaphores = {};
		std::vector<VkFence> InFlightVulkanFences = {};

		VkFormat SwapchainFormat = VK_FORMAT_UNDEFINED;
		VkExtent2D SwapchainExtent = {};
		size_t CurrentImageIndex = 0;
		unsigned int InFlightFrameCount = 0;

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
			VkDevice device,
			unsigned int inFlightFrameCount,
			VkCommandPool graphicsCommandPool);

		bool Shutdown(VkInstance instance, VkAllocationCallbacks* const allocator, VkDevice device, VkCommandPool graphicsCommandPool);

		void IncrementCurrentImageIndex();

		// Functions to set render context settings.
		void SetVSyncEnabled(const bool enabled) { VSyncEnabled = enabled; }
		void SetSwapchainImageCount(const unsigned int count) { SwapchainImageCount = count; }
	};
}