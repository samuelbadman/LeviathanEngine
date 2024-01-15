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
		if (!VulkanApi::CreateVulkanSurface(instance, platformWindowHandle, allocator, VulkanSurface))
		{
			return false;
		}

		if (!VulkanApi::CreateVulkanSwapchain(VulkanSurface,
			physicalDevice,
			swapchainColorSpace,
			swapchainFormat,
			VulkanApi::GetVSyncPresentMode(VSyncEnabled),
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

		return true;
	}

	bool RenderContextInstance::Shutdown(VkInstance instance, VkAllocationCallbacks* const allocator, VkDevice device)
	{
		VulkanApi::DestroyVulkanSurface(instance, VulkanSurface, allocator);

		for (size_t i = 0; i < static_cast<size_t>(SwapchainImageCount); ++i)
		{
			VulkanApi::DestroyVulkanFramebuffer(device, VulkanSwapchainFramebuffers[i], allocator);
			VulkanApi::DestroyVulkanImageView(device, VulkanSwapchainImageViews[i], allocator);
		}

		VulkanApi::DestroyVulkanSwapchain(device, VulkanSwapchain, allocator);

		return true;
	}
}
