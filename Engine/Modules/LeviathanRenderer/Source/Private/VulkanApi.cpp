#include "VulkanApi.h"
#include "Logging.h"
#include "LeviathanAssert.h"

namespace LeviathanRenderer
{
#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
	typedef VkBool32(VKAPI_PTR* PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);
	typedef VkFlags VkWin32SurfaceCreateFlagsKHR;
	typedef struct VkWin32SurfaceCreateInfoKHR
	{
		VkStructureType                 sType;
		const void*						pNext;
		VkWin32SurfaceCreateFlagsKHR    flags;
		HINSTANCE                       hinstance;
		HWND                            hwnd;
	} VkWin32SurfaceCreateInfoKHR;
	typedef VkResult(VKAPI_PTR* PFN_vkCreateWin32SurfaceKHR)(VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface);
#endif // LEVIATHAN_BUILD_PLATFORM_WIN32

#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
	static std::string MessageSeverityToString(const VkDebugUtilsMessageSeverityFlagBitsEXT severity)
	{
		switch (severity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			return "Error";

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			return "Info";

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			return "Verbose";

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			return "Warning";

		default:
			return "Unknown";
		}
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
		[[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
		[[maybe_unused]] const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		[[maybe_unused]] void* pUserData)
	{
		if (pCallbackData == nullptr)
		{
			return VK_FALSE;
		}

		std::string messageSeverityString = MessageSeverityToString(messageSeverity);
		LEVIATHAN_LOG("Vulkan validation layer message: Severity: %s: %s", messageSeverityString.c_str(), pCallbackData->pMessage);

		if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			LEVIATHAN_LOG("Message severity %s encountered. Asserting.", messageSeverityString.c_str());
			LEVIATHAN_ASSERT(false);
		}

		return VK_FALSE;
	}

	static void PopulateVkDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugUtilsMessengerCallback;
	}

	static bool CheckVkValidationLayerAvailable(const char* layerName)
	{
		unsigned int count = 0;
		vkEnumerateInstanceLayerProperties(&count, nullptr);
		std::vector<VkLayerProperties> availableLayers(count);
		vkEnumerateInstanceLayerProperties(&count, availableLayers.data());
		for (const VkLayerProperties& properties : availableLayers)
		{
			if (strcmp(layerName, properties.layerName) == 0)
			{
				return true;
			}
		}
		return false;
	}
#endif // LEVIATHAN_BUILD_CONFIG_DEBUG

	static bool IsPhysicalDeviceSuitable([[maybe_unused]] const VkPhysicalDeviceProperties& properties,
		[[maybe_unused]] const VkPhysicalDeviceMemoryProperties& memoryProperties, 
		[[maybe_unused]] const VkPhysicalDeviceFeatures& features)
	{
		// Check the physical device has at least one memory heap.
		if (memoryProperties.memoryHeapCount == 0)
		{
			return false;
		}

		// Check the physical device is a dedicated device.
		if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			return false;
		}

		return true;
	}

	bool VulkanPhysicalDeviceQueueFamilyIndices::Complete() const
	{
		return (Graphics.has_value()) && (Present.has_value()) && (Compute.has_value()) && (Transfer.has_value()) && (SparseBinding.has_value());
	}

	VkAllocationCallbacks* CreateVulkanAllocator()
	{
		return nullptr;
	}

	bool CreateVulkanInstance(unsigned int apiVersion, std::string_view applicationName, std::string_view engineName, const VkAllocationCallbacks* allocator, VkInstance& outInstance)
	{
		static constexpr const char* vulkanValidationLayerName = "VK_LAYER_KHRONOS_validation";

#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
		static constexpr const char* vulkanWinSurfaceExtensionName = "VK_KHR_win32_surface";
#endif // LEVIATHAN_BUILD_PLATFORM_WIN32

		VkApplicationInfo applicationInfo = {};
		applicationInfo.apiVersion = apiVersion;
		applicationInfo.pApplicationName = applicationName.data();
		applicationInfo.pEngineName = engineName.data();

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &applicationInfo;

		std::vector<const char*> enabledLayerNames;
#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
		if (CheckVkValidationLayerAvailable(vulkanValidationLayerName))
		{
			enabledLayerNames.emplace_back(vulkanValidationLayerName);
			LEVIATHAN_LOG("Vulkan instance creation: Vulkan validation layer enabled.");
		}
		else
		{
			LEVIATHAN_LOG("Vulkan instance creation: Vulkan validation layer is unavailable and not enabled.");
		}
#endif // LEVIATHAN_BUILD_CONFIG_DEBUG

		std::vector<const char*> enabledExtensionNames =
		{
	#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
			vulkanWinSurfaceExtensionName,
	#endif // LEVIATHAN_BUILD_PLATFORM_WIN32

			VK_KHR_SURFACE_EXTENSION_NAME
		};

		createInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayerNames.size());
		createInfo.ppEnabledLayerNames = enabledLayerNames.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensionNames.size());
		createInfo.ppEnabledExtensionNames = enabledExtensionNames.data();

#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {};
		PopulateVkDebugUtilsMessengerCreateInfo(debugUtilsMessengerCreateInfo);
		createInfo.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugUtilsMessengerCreateInfo);
#endif // LEVIATHAN_BUILD_CONFIG_DEBUG

		return (vkCreateInstance(&createInfo, allocator, &outInstance) == VK_SUCCESS);
	}

	void DestroyVulkanInstance(VkInstance instance, VkAllocationCallbacks* allocator)
	{
		vkDestroyInstance(instance, allocator);
	}

	bool SelectPhysicalDevice(VkInstance instance,
		VkPhysicalDeviceProperties& outPhysicalDeviceProperties, 
		VkPhysicalDeviceMemoryProperties& outPhysicalDeviceMemoryProperties, 
		VkPhysicalDeviceFeatures& outPhysicalDeviceFeatures,
		VkPhysicalDevice& outPhysicalDevice)
	{
		outPhysicalDeviceProperties = {};
		outPhysicalDeviceMemoryProperties = {};
		outPhysicalDeviceFeatures = {};

		unsigned int count = 0;
		if (vkEnumeratePhysicalDevices(instance, &count, nullptr) != VK_SUCCESS)
		{
			return false;
		}

		if (count == 0)
		{
			// No physical devices were found.
			return false;
		}

		std::vector<VkPhysicalDevice> physicalDevices(static_cast<size_t>(count), VK_NULL_HANDLE);
		if (vkEnumeratePhysicalDevices(instance, &count, physicalDevices.data()) != VK_SUCCESS)
		{
			return false;
		}

		for (size_t i = 0; i < static_cast<size_t>(count); ++i)
		{
			VkPhysicalDeviceProperties properties = {};
			vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

			VkPhysicalDeviceMemoryProperties memoryProperties = {};
			vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &memoryProperties);

			VkPhysicalDeviceFeatures features = {};
			vkGetPhysicalDeviceFeatures(physicalDevices[i], &features);

			if (!IsPhysicalDeviceSuitable(properties, memoryProperties, features))
			{
				continue;
			}

			// Select the first suitable physical device.
			outPhysicalDevice = physicalDevices[i];
			outPhysicalDeviceProperties = properties;
			outPhysicalDeviceMemoryProperties = memoryProperties;
			outPhysicalDeviceFeatures = features;

			return true;
		}

		return false;
	}

	bool GetPhysicalDeviceVideoMemorySizeGb(const VkPhysicalDeviceMemoryProperties& memoryProperties, unsigned long long& outVideoMemorySizeGb)
	{
		for (size_t i = 0; i < static_cast<size_t>(memoryProperties.memoryHeapCount); ++i)
		{
			// If the device local bit is set in the memory heap's flags the heap corresponds to device-local memory.
			if (memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			{
				outVideoMemorySizeGb = static_cast<unsigned long long>(static_cast<double>(memoryProperties.memoryHeaps[i].size) * 1e-9);
				return true;
			}
		}

		outVideoMemorySizeGb = 0;
		return false;
	}

	bool GetPhysicalDeviceQueueFamilyIndices(VkInstance const instance, VkPhysicalDevice const physicalDevice, VulkanPhysicalDeviceQueueFamilyIndices& outQueueFamilyIndices)
	{
		outQueueFamilyIndices = {};

#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
		auto pfnGetPhysicalDeviceWin32PresentationSupportKHR =
			reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR"));

		if (pfnGetPhysicalDeviceWin32PresentationSupportKHR == nullptr)
		{
			return false;
		}
#endif // LEVIATHAN_BUILD_PLATFORM_WIN32

		// When vkGetPhysicalDeviceQueueFamilyProperties is called with a null pQueueFamilyProperties parameter, it sets the value of count to the number of queue families supported
		// by the physical device. Calling vkGetPhysicalDeviceQueueFamilyProperties with a valid pQueueFamilyProperties parameter makes the function read the value of count. This
		// call just gets the count of supported families by the physical device.
		unsigned int count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);

		// This call actually retrieves the queue family properties.
		std::vector<VkQueueFamilyProperties> properties(static_cast<size_t>(count));
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, properties.data());

		for (unsigned int i = 0; i < count; ++i)
		{
			// Early exit if all queue family indices have been assigned.
			if (outQueueFamilyIndices.Complete())
			{
				return true;
			}

			// Assign i as family index for queue family type.
			if ((!outQueueFamilyIndices.Graphics.has_value()) && (properties[static_cast<size_t>(i)].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				outQueueFamilyIndices.Graphics = i;
			}
			else if ((!outQueueFamilyIndices.Compute.has_value()) && (properties[static_cast<size_t>(i)].queueFlags & VK_QUEUE_COMPUTE_BIT))
			{
				outQueueFamilyIndices.Compute = i;
			}
			else if ((!outQueueFamilyIndices.Transfer.has_value()) && (properties[static_cast<size_t>(i)].queueFlags & VK_QUEUE_TRANSFER_BIT))
			{
				outQueueFamilyIndices.Transfer = i;
			}
			else if ((!outQueueFamilyIndices.SparseBinding.has_value()) && (properties[static_cast<size_t>(i)].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT))
			{
				outQueueFamilyIndices.SparseBinding = i;
			}

			// Assign i as the present queue if queue family at index i supports presentation and has not yet been assigned.
			if (!outQueueFamilyIndices.Present.has_value())
			{
#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
				if (pfnGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, i) == VK_TRUE)
				{
					outQueueFamilyIndices.Present = i;
				}
#endif // LEVIATHAN_BUILD_PLATFORM_WIN32
			}
		}

		return outQueueFamilyIndices.Complete();
	}
}