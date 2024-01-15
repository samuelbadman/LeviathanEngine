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
		static constexpr VkFormat SwapchainFormat = /*VK_FORMAT_B8G8R8A8_SRGB */ VK_FORMAT_B8G8R8A8_UNORM;

		static VkAllocationCallbacks* VulkanAllocator = nullptr;
		static VkInstance VulkanInstance = VK_NULL_HANDLE;
		static VkPhysicalDeviceProperties VulkanPhysicalDeviceProperties = {};
		static VkPhysicalDeviceMemoryProperties VulkanPhysicalDeviceMemoryProperties = {};
		static VkPhysicalDeviceFeatures VulkanPhysicalDeviceFeatures = {};
		static VkPhysicalDevice VulkanPhysicalDevice = VK_NULL_HANDLE;
		static VulkanApi::VulkanPhysicalDeviceQueueFamilyIndices PhysicalDeviceQueueFamilyIndices = {};
		static VkDevice VulkanDevice = VK_NULL_HANDLE;
		static VkQueue VulkanGraphicsQueue = VK_NULL_HANDLE;
		static VkCommandPool VulkanGraphicsCommandPool = VK_NULL_HANDLE;
		static VkCommandBuffer VulkanGraphicsCommandBuffer = VK_NULL_HANDLE;

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
			VulkanApi::GetVulkanDeviceQueue(VulkanDevice, PhysicalDeviceQueueFamilyIndices.Graphics.value(), 0, VulkanGraphicsQueue);

			// Create command pools.
			if (!VulkanApi::CreateVulkanCommandPool(VulkanDevice, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, PhysicalDeviceQueueFamilyIndices.Graphics.value(), VulkanAllocator, VulkanGraphicsCommandPool))
			{
				return false;
			}

			// Allocate command buffers.
			if (!VulkanApi::AllocateVulkanCommandBuffer(VulkanDevice, VulkanGraphicsCommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &VulkanGraphicsCommandBuffer))
			{
				return false;
			}

			return true;
		}

		bool Shutdown()
		{
			VulkanApi::DestroyVulkanCommandPool(VulkanDevice, VulkanGraphicsCommandPool, VulkanAllocator);
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
				VulkanDevice);
		}

		bool ShutdownRenderContextInstance(RenderContextInstance* const context)
		{
			return context->Shutdown(VulkanInstance, VulkanAllocator, VulkanDevice);
		}
	}
}