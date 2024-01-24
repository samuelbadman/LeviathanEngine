#include "RenderContext.h"
#include "VulkanRenderContext.h"
#include "VulkanApi.h"

namespace LeviathanRenderer
{
	namespace RenderContext
	{
		RenderContextSettings& GetRenderContextSettings(RenderContextInstance* context)
		{
			return context->GetSettings();
		}
	}

	bool RenderContextInstance::Initialize(VkInstance instance,
		void* platformWindowHandle,
		VkAllocationCallbacks* const allocator,
		VkPhysicalDevice physicalDevice,
		VkColorSpaceKHR swapchainColorSpace,
		VkFormat swapchainFormat,
		VkDevice device,
		unsigned int inFlightFrameCount,
		VkCommandPool graphicsCommandPool,
		VkRenderPass mainRenderPass)
	{
		// Create the surface.
		if (!VulkanApi::CreateVulkanSurface(instance, platformWindowHandle, allocator, VulkanSurface))
		{
			return false;
		}

		// Get the surface capabilities.
		if (!VulkanApi::GetVulkanPhysicalDeviceSurfaceCapabilities(physicalDevice, VulkanSurface, VulkanSurfaceCapabilities))
		{
			return false;
		}

		// Get the surface formats.
		if (!VulkanApi::GetVulkanPhysicalDeviceSurfaceFormats(physicalDevice, VulkanSurface, VulkanSurfaceFormats))
		{
			return false;
		}

		// Get the surface present modes.
		if (!VulkanApi::GetVulkanPhysicalDeviceSurfacePresentModes(physicalDevice, VulkanSurface, VulkanSurfacePresentModes))
		{
			return false;
		}

		// Create the swapchain.
		if (!CreateSwapchain(allocator, swapchainColorSpace, swapchainFormat, Settings.SwapChainBackBufferCount, device, mainRenderPass))
		{
			return false;
		}

		// Allocate command buffers.
		GraphicsVulkanCommandBuffers.resize(static_cast<size_t>(inFlightFrameCount));

		if (!VulkanApi::AllocateVulkanCommandBuffers(device,
			graphicsCommandPool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			inFlightFrameCount,
			GraphicsVulkanCommandBuffers.data()))
		{
			return false;
		}

		// Create synchronization objects.
		ImageAvailableVulkanSemaphores.resize(static_cast<size_t>(inFlightFrameCount));
		RenderFinishedVulkanSemaphores.resize(static_cast<size_t>(inFlightFrameCount));
		InFlightVulkanFences.resize(static_cast<size_t>(inFlightFrameCount));

		for (size_t i = 0; i < static_cast<size_t>(inFlightFrameCount); ++i)
		{
			if (!VulkanApi::CreateVulkanSemaphore(device, allocator, ImageAvailableVulkanSemaphores[i]))
			{
				return false;
			}

			if (!VulkanApi::CreateVulkanSemaphore(device, allocator, RenderFinishedVulkanSemaphores[i]))
			{
				return false;
			}

			if (!VulkanApi::CreateVulkanFence(device, allocator, InFlightVulkanFences[i]))
			{
				return false;
			}
		}

		// Store in flight frame count.
		InFlightFrameCount = inFlightFrameCount;

		return true;
	}

	bool RenderContextInstance::Shutdown(VkInstance instance, VkAllocationCallbacks* const allocator, VkDevice device, VkCommandPool graphicsCommandPool)
	{
		DestroySwapchain(device, allocator);

		VulkanApi::DestroyVulkanSurface(instance, VulkanSurface, allocator);

		VulkanApi::FreeCommandBuffers(device, graphicsCommandPool, InFlightFrameCount, GraphicsVulkanCommandBuffers.data());

		for (size_t i = 0; i < static_cast<size_t>(InFlightFrameCount); ++i)
		{
			VulkanApi::DestroyVulkanSemaphore(device, ImageAvailableVulkanSemaphores[i], allocator);
			VulkanApi::DestroyVulkanSemaphore(device, RenderFinishedVulkanSemaphores[i], allocator);
			VulkanApi::DestroyVulkanFence(device, InFlightVulkanFences[i], allocator);
		}

		return true;
	}

	void RenderContextInstance::IncrementCurrentInFlightFrameIndex()
	{
		CurrentInFlightFrameIndex = (CurrentInFlightFrameIndex + 1) % static_cast<size_t>(InFlightFrameCount);
	}

	bool RenderContextInstance::AcquireCurrentImageIndex(VkDevice device, unsigned long long timeoutDurationNanoseconds)
	{
		// Acquire the context's swapchain's next image index. This function signals the specified semaphore and/or fence once the presentation engine has finished with
		// the image resource and the image memory is available to be rendered to.
		return VulkanApi::AcquireNextImageFromSwapchain(device,
			VulkanSwapchain,
			timeoutDurationNanoseconds,
			ImageAvailableVulkanSemaphores[CurrentInFlightFrameIndex],
			VK_NULL_HANDLE,
			CurrentImageIndex);
	}

	bool RenderContextInstance::RecreateSwapchain(VkDevice device,
		VkColorSpaceKHR swapchainColorSpace,
		VkFormat swapchainFormat,
		VkRenderPass mainRenderPass,
		VkAllocationCallbacks* const allocator)
	{
		DestroySwapchain(device, allocator);

		return CreateSwapchain(allocator, swapchainColorSpace, swapchainFormat, Settings.SwapChainBackBufferCount, device, mainRenderPass);
	}

	bool RenderContextInstance::CreateSwapchain(VkAllocationCallbacks* const allocator,
		VkColorSpaceKHR swapchainColorSpace,
		VkFormat swapchainFormat,
		unsigned int swapchainImageCount,
		VkDevice device,
		VkRenderPass mainRenderPass)
	{
		if (!VulkanApi::CreateVulkanSwapchain(VulkanSurface,
			VulkanSurfaceCapabilities,
			VulkanSurfaceFormats.size(),
			VulkanSurfaceFormats.data(),
			VulkanSurfacePresentModes.size(),
			VulkanSurfacePresentModes.data(),
			swapchainColorSpace,
			swapchainFormat,
			VulkanApi::GetPresentModeForVSyncState(Settings.VSyncEnabled),
			VK_NULL_HANDLE,
			swapchainImageCount,
			device,
			allocator,
			VulkanSwapchain,
			SwapchainExtent,
			SwapchainFormat))
		{
			return false;
		}

		// Retreive the swapchain images.
		VulkanSwapchainImages.resize(static_cast<size_t>(Settings.SwapChainBackBufferCount));

		if (!VulkanApi::RetreiveSwapchainImages(device, VulkanSwapchain, VulkanSwapchainImages))
		{
			return false;
		}

		// Create swapchain image views and framebuffers.
		VulkanSwapchainImageViews.resize(static_cast<size_t>(Settings.SwapChainBackBufferCount));
		VulkanSwapchainFramebuffers.resize(static_cast<size_t>(Settings.SwapChainBackBufferCount));

		for (size_t i = 0; i < static_cast<size_t>(Settings.SwapChainBackBufferCount); ++i)
		{
			if (!VulkanApi::CreateVulkanImageView(device,
				VulkanSwapchainImages[i],
				VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
				SwapchainFormat,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VkComponentMapping
				{
					.r = VK_COMPONENT_SWIZZLE_IDENTITY,
					.g = VK_COMPONENT_SWIZZLE_IDENTITY,
					.b = VK_COMPONENT_SWIZZLE_IDENTITY,
					.a = VK_COMPONENT_SWIZZLE_IDENTITY
				},
				allocator,
				VulkanSwapchainImageViews[i]))
			{
				return false;
			}

			if (!VulkanApi::CreateVulkanFramebuffer(device,
				mainRenderPass,
				1,
				&VulkanSwapchainImageViews[i],
				SwapchainExtent.width,
				SwapchainExtent.height,
				allocator,
				VulkanSwapchainFramebuffers[i]))
			{
				return false;
			}
		}

		return true;
	}

	void RenderContextInstance::DestroySwapchain(VkDevice device, VkAllocationCallbacks* const allocator)
	{
		VulkanApi::DestroyVulkanSwapchain(device, VulkanSwapchain, allocator);

		for (size_t i = 0; i < static_cast<size_t>(Settings.SwapChainBackBufferCount); ++i)
		{
			VulkanApi::DestroyVulkanFramebuffer(device, VulkanSwapchainFramebuffers[i], allocator);
			VulkanApi::DestroyVulkanImageView(device, VulkanSwapchainImageViews[i], allocator);
		}
	}
}
