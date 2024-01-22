#include "RenderDevice.h"
#include "VulkanApi.h"
#include "Logging.h"
#include "VulkanRenderContext.h"
#include "Core.h"
#include "Timing.h"

namespace LeviathanRenderer
{
	namespace RenderDevice
	{
		static constexpr VkColorSpaceKHR SwapchainColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		static constexpr VkFormat SwapchainFormat = /* VK_FORMAT_B8G8R8A8_SRGB */ VK_FORMAT_B8G8R8A8_UNORM;
		static constexpr unsigned int RenderContextInstanceInFlightFrameCount = 3;

		static VkAllocationCallbacks* VulkanAllocator = nullptr;
		static VkInstance VulkanInstance = VK_NULL_HANDLE;
		static VkPhysicalDeviceProperties VulkanPhysicalDeviceProperties = {};
		static VkPhysicalDeviceMemoryProperties VulkanPhysicalDeviceMemoryProperties = {};
		static VkPhysicalDeviceFeatures VulkanPhysicalDeviceFeatures = {};
		static VkPhysicalDevice VulkanPhysicalDevice = VK_NULL_HANDLE;
		static VulkanApi::VulkanPhysicalDeviceQueueFamilyIndices PhysicalDeviceQueueFamilyIndices = {};
		static VkDevice VulkanDevice = VK_NULL_HANDLE;
		static VkQueue GraphicsVulkanQueue = VK_NULL_HANDLE;
		static VkCommandPool GraphicsVulkanCommandPool = VK_NULL_HANDLE;
		static VkRenderPass MainVulkanRenderPass = VK_NULL_HANDLE;

		bool Initialize()
		{
			// Create allocator.
			VulkanAllocator = VulkanApi::CreateVulkanAllocator();

			// Create instance.
			if (!VulkanApi::CreateVulkanInstance(VK_API_VERSION_1_3, "LeviathanApplication", "LeviathanEngine", VulkanAllocator, VulkanInstance))
			{
				return false;
			}

			// Select physical device.
			if (!VulkanApi::SelectVulkanPhysicalDevice(VulkanInstance, VulkanPhysicalDeviceProperties, VulkanPhysicalDeviceMemoryProperties, VulkanPhysicalDeviceFeatures, VulkanPhysicalDevice))
			{
				return false;
			}

			unsigned long long availableVideoMemoryGb = 0;
			if (!VulkanApi::GetVulkanPhysicalDeviceVideoMemorySizeGb(VulkanPhysicalDeviceMemoryProperties, availableVideoMemoryGb))
			{
				return false;
			}

			LEVIATHAN_LOG("Vulkan render device: Selected physical device: Name %s, Dedicated video memory %zu GB", VulkanPhysicalDeviceProperties.deviceName, availableVideoMemoryGb);

			if (!GetVulkanPhysicalDeviceQueueFamilyIndices(VulkanPhysicalDevice, PhysicalDeviceQueueFamilyIndices))
			{
				return false;
			}

			if (!VulkanApi::IsPresentationSupportedOnQueue(VulkanInstance, VulkanPhysicalDevice, PhysicalDeviceQueueFamilyIndices.Graphics.value()))
			{
				return false;
			}

			// Create logical device.
			VulkanApi::VulkanDeviceQueueCountAndPriorities graphicsQueueCountAndPriorities = {};
			graphicsQueueCountAndPriorities.QueueCount = 1;
			graphicsQueueCountAndPriorities.QueuePriorities.emplace_back(1.0f);

			VulkanApi::VulkanDeviceQueueCountAndPriorities EmptyQueueCountAndPriorities = {};

			if (!CreateVulkanLogicalDevice(VulkanPhysicalDevice,
				PhysicalDeviceQueueFamilyIndices,
				graphicsQueueCountAndPriorities,
				EmptyQueueCountAndPriorities,
				EmptyQueueCountAndPriorities,
				EmptyQueueCountAndPriorities,
				VulkanAllocator,
				VulkanDevice))
			{
				return false;
			}

			// Get device queues.
			VulkanApi::GetVulkanDeviceQueue(VulkanDevice, PhysicalDeviceQueueFamilyIndices.Graphics.value(), 0, GraphicsVulkanQueue);

			// Create command pools.
			if (!VulkanApi::CreateVulkanCommandPool(VulkanDevice,
				VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
				PhysicalDeviceQueueFamilyIndices.Graphics.value(),
				VulkanAllocator,
				GraphicsVulkanCommandPool))
			{
				return false;
			}

			// Create render passes.
			VkAttachmentDescription colorAttachmentDescription = {};
			colorAttachmentDescription.format = SwapchainFormat;
			colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference colorAttachmentReference = {};
			colorAttachmentReference.attachment = 0;
			colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription mainRenderPassSubpassDescription = {};
			mainRenderPassSubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			mainRenderPassSubpassDescription.colorAttachmentCount = 1;
			mainRenderPassSubpassDescription.pColorAttachments = &colorAttachmentReference;

			VkSubpassDependency mainRenderPassSubpassDependency = {};
			mainRenderPassSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			mainRenderPassSubpassDependency.dstSubpass = 0;
			mainRenderPassSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			mainRenderPassSubpassDependency.srcAccessMask = 0;
			mainRenderPassSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			mainRenderPassSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			VkRenderPassCreateInfo mainRenderPassCreateInfo = {};
			mainRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			mainRenderPassCreateInfo.attachmentCount = 1;
			mainRenderPassCreateInfo.pAttachments = &colorAttachmentDescription;
			mainRenderPassCreateInfo.subpassCount = 1;
			mainRenderPassCreateInfo.pSubpasses = &mainRenderPassSubpassDescription;
			mainRenderPassCreateInfo.dependencyCount = 1;
			mainRenderPassCreateInfo.pDependencies = &mainRenderPassSubpassDependency;

			if (vkCreateRenderPass(VulkanDevice, &mainRenderPassCreateInfo, VulkanAllocator, &MainVulkanRenderPass) != VK_SUCCESS)
			{
				return false;
			}

			return true;
		}

		bool Shutdown()
		{
			// All render context instances created by this render device must be shutdown and destroyed before shutting down this render device.

			// Wait for all queues on the logical device to finish outstanding queue operations before destroying the logical device.
			if (vkDeviceWaitIdle(VulkanDevice) != VK_SUCCESS)
			{
				return false;
			}

			vkDestroyRenderPass(VulkanDevice, MainVulkanRenderPass, VulkanAllocator);
			VulkanApi::DestroyVulkanCommandPool(VulkanDevice, GraphicsVulkanCommandPool, VulkanAllocator);
			VulkanApi::DestroyVulkanLogicalDevice(VulkanDevice, VulkanAllocator);
			VulkanApi::DestroyVulkanInstance(VulkanInstance, VulkanAllocator);

			return true;
		}

		RenderContextInstance* CreateRenderContextInstance()
		{
			return new RenderContextInstance();
		}

		void DestroyRenderContextInstance(RenderContextInstance* const context)
		{
			delete context;
		}

		bool InitializeRenderContextInstance(RenderContextInstance* const context)
		{
			return context->Initialize(VulkanInstance,
				LeviathanCore::Core::GetRuntimeWindowPlatformHandle(),
				VulkanAllocator,
				VulkanPhysicalDevice,
				SwapchainColorSpace,
				SwapchainFormat,
				VulkanDevice,
				RenderContextInstanceInFlightFrameCount,
				GraphicsVulkanCommandPool,
				MainVulkanRenderPass);
		}

		bool ShutdownRenderContextInstance(RenderContextInstance* const context)
		{
			// Wait for the graphics queue to finish doing work with the context's resources. TODO: Potentially create a queue per context to be able to 
			// only wait for the work for the context being shutdown instead of all graphics queue work.
			if (vkQueueWaitIdle(GraphicsVulkanQueue) != VK_SUCCESS)
			{
				return false;
			}

			return context->Shutdown(VulkanInstance, VulkanAllocator, VulkanDevice, GraphicsVulkanCommandPool);
		}

		bool BeginFrameCommandRecording(RenderContextInstance* const context)
		{
			// Obtain context resources for current in flight frame.
			const VkFence currentInFlightFrameFence = context->GetCurrentInFlightFrameFence();
			const VkCommandBuffer GraphicsCommandBufferForCurrentInFlightFrame = context->GetGraphicsCommandBufferForCurrentInFlightFrame();

			//LEVIATHAN_LOG("Waiting for in flight fence %d", context->GetCurrentInFlightFrameIndex());

			// Wait for the context's current in flight frame resources to be finished with from the previous frame before resetting them for this frame.
			if (!VulkanApi::WaitForFences(VulkanDevice, 1, &currentInFlightFrameFence, VK_TRUE, static_cast<unsigned long long>(LeviathanCore::Timing::MaxNanoseconds)))
			{
				return false;
			}

			//LEVIATHAN_LOG("Finished waiting for in flight fence %d", context->GetCurrentInFlightFrameIndex());

			// Acquire next image index for the current frame. Next image index is stored within the render context instance as the current image index.
			if (!context->AcquireCurrentImageIndex(VulkanDevice, static_cast<unsigned long long>(LeviathanCore::Timing::MaxNanoseconds)))
			{
				return false;
			}

			// Reset the current in flight frame's graphics command buffer to its initial state.
			if (!VulkanApi::ResetCommandBuffer(GraphicsCommandBufferForCurrentInFlightFrame))
			{
				return false;
			}

			// Begin the current in flight frame's graphics command buffer to start recording rendering commands.
			if (!VulkanApi::BeginCommandBuffer(GraphicsCommandBufferForCurrentInFlightFrame))
			{
				return false;
			}

			// Start main render pass.
			VkClearValue clearColor =
			{
				.color = {1.0f, 0.0f, 0.0f, 1.0f}
			};

			VkRenderPassBeginInfo mainRenderPassBeginInfo = {};
			mainRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			mainRenderPassBeginInfo.renderPass = MainVulkanRenderPass;
			mainRenderPassBeginInfo.framebuffer = context->GetCurrentImageFramebuffer();
			mainRenderPassBeginInfo.renderArea.offset = 
			{ 
				.x = 0,
				.y = 0
			};
			mainRenderPassBeginInfo.renderArea.extent = context->GetSwapchainExtent();
			mainRenderPassBeginInfo.clearValueCount = 1;
			mainRenderPassBeginInfo.pClearValues = &clearColor;
			vkCmdBeginRenderPass(GraphicsCommandBufferForCurrentInFlightFrame, &mainRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			return true;
		}

		bool EndFrameCommandRecording(RenderContextInstance* const context)
		{
			vkCmdEndRenderPass(context->GetGraphicsCommandBufferForCurrentInFlightFrame());

			return VulkanApi::EndCommandBuffer(context->GetGraphicsCommandBufferForCurrentInFlightFrame());
		}

		bool SubmitRecordedFrameCommands(RenderContextInstance* const context)
		{
			// Reset the fence to an unsignalled state now that work is being submitted for the frame.
			const VkFence inFlightFrameFence = context->GetCurrentInFlightFrameFence();

			if (!VulkanApi::ResetFences(VulkanDevice, 1, &inFlightFrameFence))
			{
				return false;
			}

			// Submit command buffer for current in flight frame. This sets the current in flight frame's render finished semaphore to be signalled once the GPU has 
			// finished processing the submitted command buffer. The current in flight frame's fence is also signalled once the GPU has finished processing the submitted
			// command buffer.
			const VkSemaphore waitSemaphore = context->GetImageAvailableSemaphoreForCurrentInFlightFrame();
			const VkPipelineStageFlags waitPipelineStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			const VkCommandBuffer submitGraphicsCommandBuffer = context->GetGraphicsCommandBufferForCurrentInFlightFrame();
			const VkSemaphore signalSemaphore = context->GetRenderFinishedSemaphoreForCurrentInFlightFrame();

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &waitSemaphore;
			submitInfo.pWaitDstStageMask = &waitPipelineStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &submitGraphicsCommandBuffer;
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &signalSemaphore;

			return VulkanApi::VulkanQueueSubmit(GraphicsVulkanQueue, 1, &submitInfo, context->GetCurrentInFlightFrameFence());
		}

		bool Present(RenderContextInstance* const context)
		{
			const VkSemaphore waitSemaphore = context->GetRenderFinishedSemaphoreForCurrentInFlightFrame();
			const VkSwapchainKHR presentSwapchain = context->GetSwapchain();
			const unsigned int presentSwapchainImageIndex = context->GetCurrentImageIndex();

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &waitSemaphore;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &presentSwapchain;
			presentInfo.pImageIndices = &presentSwapchainImageIndex;
			presentInfo.pResults = nullptr;

			// Progress the context's current in flight frame.
			context->IncrementCurrentInFlightFrameIndex();

			return VulkanApi::VulkanQueuePresent(GraphicsVulkanQueue, &presentInfo);
		}

		bool ResizeRenderContextInstance(RenderContextInstance* const context)
		{
			if (vkQueueWaitIdle(GraphicsVulkanQueue) != VK_SUCCESS)
			{
				return false;
			}

			return context->RecreateSwapchain(VulkanDevice, SwapchainColorSpace, SwapchainFormat, MainVulkanRenderPass, VulkanAllocator);
		}

		namespace RenderCommands
		{

		}
	}
}