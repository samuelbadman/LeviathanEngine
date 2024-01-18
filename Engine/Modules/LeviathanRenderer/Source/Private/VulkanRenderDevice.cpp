#include "RenderDevice.h"
#include "VulkanApi.h"
#include "Logging.h"
#include "VulkanRenderContext.h"
#include "Core.h"

namespace LeviathanRenderer
{
	namespace RenderDevice
	{
		static constexpr VkColorSpaceKHR SwapchainColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		static constexpr VkFormat SwapchainFormat = /* VK_FORMAT_B8G8R8A8_SRGB */ VK_FORMAT_B8G8R8A8_UNORM;

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
				3,
				GraphicsVulkanCommandPool);
		}

		bool ShutdownRenderContextInstance(RenderContextInstance* const context)
		{
			return context->Shutdown(VulkanInstance, VulkanAllocator, VulkanDevice, GraphicsVulkanCommandPool);
		}
	}

	namespace RenderCommands
	{
		bool CmdBeginFrame(VkCommandBuffer commandBuffer)
		{
			if (!VulkanApi::BeginCommandBuffer(commandBuffer))
			{
				return false;
			}

			 

			return true;
		}
	}
}