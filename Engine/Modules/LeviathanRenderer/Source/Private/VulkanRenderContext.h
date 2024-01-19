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
		size_t CurrentInFlightFrameIndex = 0;
		unsigned int InFlightFrameCount = 0;
		unsigned int CurrentImageIndex = 0;

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

		void IncrementCurrentInFlightFrameIndex();

		// Acquires the index of the next swapchain image to render into. The index value is stored internally inside the VulkanRenderContext. This function signals the 
		// image available semaphore for the current in flight frame index once the image becomes available meaning that this function must be called after the current in 
		// flight frame index has been progressed, as the function signals the semaphore resource for the current in flight frame index.
		bool AcquireCurrentImageIndex(VkDevice device, unsigned long long timeoutDurationNanoseconds);

		VkCommandBuffer GetGraphicsCommandBufferForCurrentInFlightFrame() const { return GraphicsVulkanCommandBuffers[static_cast<size_t>(CurrentInFlightFrameIndex)]; }

		VkFence GetCurrentInFlightFrameFence() const { return InFlightVulkanFences[static_cast<size_t>(CurrentInFlightFrameIndex)]; }

		VkSwapchainKHR GetSwapchain() const { return VulkanSwapchain; }

		VkSemaphore GetImageAvailableSemaphoreForCurrentInFlightFrame() const { return ImageAvailableVulkanSemaphores[static_cast<size_t>(CurrentInFlightFrameIndex)]; }

		VkSemaphore GetRenderFinishedSemaphoreForCurrentInFlightFrame() const { return RenderFinishedVulkanSemaphores[static_cast<size_t>(CurrentInFlightFrameIndex)]; }

		unsigned int GetCurrentImageIndex() const { return CurrentImageIndex; }

		// Functions to set render context settings.
		void SetVSyncEnabled(const bool enabled) { VSyncEnabled = enabled; }
		void SetSwapchainImageCount(const unsigned int count) { SwapchainImageCount = count; }
	};
}