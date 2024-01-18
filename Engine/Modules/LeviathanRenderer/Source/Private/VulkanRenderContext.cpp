#include "RenderContext.h"
#include "VulkanRenderContext.h"
#include "VulkanApi.h"

namespace LeviathanRenderer
{
	namespace RenderContext
	{
		void SetVSyncEnabled(RenderContextInstance* const context, const bool vsyncEnabled)
		{
			context->SetVSyncEnabled(vsyncEnabled);
		}

		void SetBackBufferCount(RenderContextInstance* const context, const unsigned int count)
		{
			context->SetSwapchainImageCount(count);
		}

		[[maybe_unused]] bool ApplyRenderContextSettings([[maybe_unused]] RenderContextInstance* const context)
		{
			// TODO: Recreate swapchain resources.
			return true;
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
		VkCommandPool graphicsCommandPool)
	{
		if (!VulkanApi::CreateVulkanSurface(instance, platformWindowHandle, allocator, VulkanSurface))
		{
			return false;
		}

		if (!VulkanApi::CreateVulkanSwapchain(VulkanSurface,
			physicalDevice,
			swapchainColorSpace,
			swapchainFormat,
			VulkanApi::GetPresentModeForVSyncState(VSyncEnabled),
			VK_NULL_HANDLE,
			SwapchainImageCount,
			device,
			allocator,
			VulkanSwapchain,
			SwapchainExtent,
			SwapchainFormat))
		{
			return false;
		}

		if (!VulkanApi::RetreiveSwapchainImages(device, VulkanSwapchain, VulkanSwapchainImages))
		{
			return false;
		}

		VulkanSwapchainImageViews.resize(static_cast<size_t>(SwapchainImageCount));
		VulkanSwapchainFramebuffers.resize(static_cast<size_t>(SwapchainImageCount));

		for (size_t i = 0; i < static_cast<size_t>(SwapchainImageCount); ++i)
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
				VK_NULL_HANDLE,
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
		VulkanApi::DestroyVulkanSurface(instance, VulkanSurface, allocator);

		VulkanApi::DestroyVulkanSwapchain(device, VulkanSwapchain, allocator);

		for (size_t i = 0; i < static_cast<size_t>(SwapchainImageCount); ++i)
		{
			VulkanApi::DestroyVulkanFramebuffer(device, VulkanSwapchainFramebuffers[i], allocator);
			VulkanApi::DestroyVulkanImageView(device, VulkanSwapchainImageViews[i], allocator);
		}

		VulkanApi::FreeCommandBuffers(device, graphicsCommandPool, InFlightFrameCount, GraphicsVulkanCommandBuffers.data());

		for (size_t i = 0; i < static_cast<size_t>(InFlightFrameCount); ++i)
		{
			VulkanApi::DestroyVulkanSemaphore(device, ImageAvailableVulkanSemaphores[i], allocator);
			VulkanApi::DestroyVulkanSemaphore(device, RenderFinishedVulkanSemaphores[i], allocator);
			VulkanApi::DestroyVulkanFence(device, InFlightVulkanFences[i], allocator);
		}

		return true;
	}

	void RenderContextInstance::IncrementCurrentImageIndex()
	{
		CurrentImageIndex = (CurrentImageIndex + 1) % static_cast<size_t>(SwapchainImageCount);
	}
}
