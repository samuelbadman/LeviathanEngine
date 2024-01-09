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

		void SetBackingBufferCount(RenderContextInstance* const context, const unsigned int count)
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
		VkDevice device)
	{
		if (!CreateVulkanSurface(instance, platformWindowHandle, allocator, VulkanSurface))
		{
			return false;
		}

		if (!CreateVulkanSwapchain(VulkanSurface,
			physicalDevice,
			swapchainColorSpace,
			swapchainFormat,
			((VSyncEnabled) ? PresentModeVSyncEnabled : PresentModeVSyncDisabled),
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

		if (!RetreiveSwapchainImages(device, VulkanSwapchain, VulkanSwapchainImages))
		{
			return false;
		}

		for (size_t i = 0; i < static_cast<size_t>(SwapchainImageCount); ++i)
		{
			if (!CreateVulkanImageView(device,
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
		}

		return true;
	}

	bool RenderContextInstance::Shutdown(VkInstance instance, VkAllocationCallbacks* const allocator, VkDevice device)
	{
		DestroyVulkanSurface(instance, VulkanSurface, allocator);

		for (size_t i = 0; i < static_cast<size_t>(SwapchainImageCount); ++i)
		{
			DestroyVulkanImageView(device, VulkanSwapchainImageViews[i], allocator);
		}

		DestroyVulkanSwapchain(device, VulkanSwapchain, allocator);

		return true;
	}
}
