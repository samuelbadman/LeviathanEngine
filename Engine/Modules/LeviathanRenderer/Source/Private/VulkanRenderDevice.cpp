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

			return true;
		}

		bool Shutdown()
		{
			// All render context instances created by this render device must be shutdown and destroyed before shutting down this render device.

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
				GraphicsVulkanCommandPool);
		}

		bool ShutdownRenderContextInstance(RenderContextInstance* const context)
		{
			return context->Shutdown(VulkanInstance, VulkanAllocator, VulkanDevice, GraphicsVulkanCommandPool);
		}

		bool BeginFrameCommandRecording(RenderContextInstance* const context)
		{
			// Progress the context's current in flight frame.
			context->IncrementCurrentInFlightFrameIndex();

			// Obtain context resources for current in flight frame.
			const VkFence currentInFlightFrameFence = context->GetCurrentInFlightFrameFence();
			const VkCommandBuffer GraphicsCommandBufferForCurrentInFlightFrame = context->GetGraphicsCommandBufferForCurrentInFlightFrame();

			// Wait for the context's current in flight frame resources to be finished with from the previous frame before resetting them for this frame.
			if (!VulkanApi::WaitForFences(VulkanDevice, 1, &currentInFlightFrameFence, VK_TRUE, static_cast<unsigned long long>(LeviathanCore::Timing::MaxNanoseconds)))
			{
				return false;
			}

			// Reset the fence to an unsignalled state now that it has been signalled by the command queue.
			if (!VulkanApi::ResetFences(VulkanDevice, 1, &currentInFlightFrameFence))
			{
				return false;
			}

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

			return true;
		}

		bool EndFrameCommandRecording(RenderContextInstance* const context)
		{
			return VulkanApi::EndCommandBuffer(context->GetGraphicsCommandBufferForCurrentInFlightFrame());
		}

		bool SubmitRecordedFrameCommands(RenderContextInstance* const context)
		{
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

			return VulkanApi::VulkanQueuePresent(GraphicsVulkanQueue, &presentInfo);
		}

		namespace RenderCommands
		{

		}
	}
}