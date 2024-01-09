#include "VulkanApi.h"
#include "Logging.h"
#include "LeviathanAssert.h"

#ifdef max
#undef max
#endif // max

namespace LeviathanRenderer
{
#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
	typedef VkBool32(VKAPI_PTR* PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);
	typedef VkFlags VkWin32SurfaceCreateFlagsKHR;
	typedef struct VkWin32SurfaceCreateInfoKHR
	{
		VkStructureType                 sType;
		const void* pNext;
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
		return (Graphics.has_value()) &&
			(Compute.has_value()) &&
			(Transfer.has_value()) &&
			(SparseBinding.has_value());
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

		createInfo.enabledLayerCount = static_cast<unsigned int>(enabledLayerNames.size());
		createInfo.ppEnabledLayerNames = enabledLayerNames.data();
		createInfo.enabledExtensionCount = static_cast<unsigned int>(enabledExtensionNames.size());
		createInfo.ppEnabledExtensionNames = enabledExtensionNames.data();

#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {};
		PopulateVkDebugUtilsMessengerCreateInfo(debugUtilsMessengerCreateInfo);
		createInfo.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugUtilsMessengerCreateInfo);
#endif // LEVIATHAN_BUILD_CONFIG_DEBUG

		return (vkCreateInstance(&createInfo, allocator, &outInstance) == VK_SUCCESS);
	}

	void DestroyVulkanInstance(VkInstance instance, const VkAllocationCallbacks* allocator)
	{
		vkDestroyInstance(instance, allocator);
	}

	bool SelectVulkanPhysicalDevice(VkInstance instance,
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

	bool GetVulkanPhysicalDeviceVideoMemorySizeGb(const VkPhysicalDeviceMemoryProperties& memoryProperties, unsigned long long& outVideoMemorySizeGb)
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

	bool GetVulkanPhysicalDeviceQueueFamilyIndices(VkPhysicalDevice const physicalDevice, VulkanPhysicalDeviceQueueFamilyIndices& outQueueFamilyIndices)
	{
		outQueueFamilyIndices = {};

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
		}

		return outQueueFamilyIndices.Complete();
	}

	bool IsPresentationSupportedOnQueue(VkInstance instance, VkPhysicalDevice physicalDevice, const unsigned int queueFamilyIndex)
	{
#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
		auto pfnGetPhysicalDeviceWin32PresentationSupportKHR =
			reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR"));

		if (pfnGetPhysicalDeviceWin32PresentationSupportKHR == nullptr)
		{
			return false;
		}

		return (pfnGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex) == VK_TRUE);
#else
		return false;
#endif // LEVIATHAN_BUILD_PLATFORM_WIN32
	}

	bool CreateVulkanLogicalDevice(VkPhysicalDevice const physicalDevice,
		const VulkanPhysicalDeviceQueueFamilyIndices& queueFamilyIndices,
		const VulkanDeviceQueueCountAndPriorities& graphicsQueueCountAndPriorities,
		const VulkanDeviceQueueCountAndPriorities& computeQueueCountAndPriorities,
		const VulkanDeviceQueueCountAndPriorities& transferQueueCountAndPriorities,
		const VulkanDeviceQueueCountAndPriorities& sparseBindingQueueCountAndPriorities,
		const VkAllocationCallbacks* const allocator,
		VkDevice& outDevice)
	{
		// Create device queue create infos.
		std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos = {};
		deviceQueueCreateInfos.reserve(4);

		auto addDeviceQueueCreateInfo = [&deviceQueueCreateInfos](unsigned int queueFamilyIndex, unsigned int queueCount, const float* pQueuePriorities)
			{
				VkDeviceQueueCreateInfo& createInfo = deviceQueueCreateInfos.emplace_back();
				createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				createInfo.queueFamilyIndex = queueFamilyIndex;
				createInfo.queueCount = queueCount;
				createInfo.pQueuePriorities = pQueuePriorities;
			};

		if (graphicsQueueCountAndPriorities.QueueCount > 0)
		{
			addDeviceQueueCreateInfo(queueFamilyIndices.Graphics.value(), graphicsQueueCountAndPriorities.QueueCount, graphicsQueueCountAndPriorities.QueuePriorities.data());
		}

		if (computeQueueCountAndPriorities.QueueCount > 0)
		{
			addDeviceQueueCreateInfo(queueFamilyIndices.Compute.value(), computeQueueCountAndPriorities.QueueCount, computeQueueCountAndPriorities.QueuePriorities.data());
		}

		if (transferQueueCountAndPriorities.QueueCount > 0)
		{
			addDeviceQueueCreateInfo(queueFamilyIndices.Transfer.value(), transferQueueCountAndPriorities.QueueCount, transferQueueCountAndPriorities.QueuePriorities.data());
		}

		if (sparseBindingQueueCountAndPriorities.QueueCount > 0)
		{
			addDeviceQueueCreateInfo(queueFamilyIndices.SparseBinding.value(), sparseBindingQueueCountAndPriorities.QueueCount, sparseBindingQueueCountAndPriorities.QueuePriorities.data());
		}

		// Add device extensions.
		// VK_KHR_timeline_semaphore extension is now part of core and the extension no longer needs to be added however, the timeline semaphore feature still
		// needs to be enabled to be used.
		std::array<const char*, 1> enabledDeviceExtensionNames =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		// Enable timeline semaphores feature from Vulkan 1.2 features. Feature is enabled by pointing to this structure in the pNext field of VkDeviceCreateInfo.
		VkPhysicalDeviceVulkan12Features vulkan12Features = {};
		vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		vulkan12Features.timelineSemaphore = true;

		// Create device.
		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
		deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensionNames.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensionNames.size());
		deviceCreateInfo.pNext = &vulkan12Features;

		return (vkCreateDevice(physicalDevice, &deviceCreateInfo, allocator, &outDevice) == VK_SUCCESS);
	}

	void GetVulkanDeviceQueue(VkDevice device, const unsigned int queueFamilyIndex, const unsigned int queueIndex, VkQueue& outQueue)
	{
		vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &outQueue);
	}

	void DestroyVulkanLogicalDevice(VkDevice device, const VkAllocationCallbacks* allocator)
	{
		vkDestroyDevice(device, allocator);
	}

	bool CreateVulkanSurface(VkInstance const instance, void* const platformWindowHandle, VkAllocationCallbacks* const allocator, VkSurfaceKHR& outSurface)
	{
#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
		auto pfnCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR"));
		if (pfnCreateWin32SurfaceKHR == nullptr)
		{
			return false;
		}

		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = static_cast<HWND>(platformWindowHandle);
		createInfo.hinstance = nullptr;
		createInfo.flags = 0;

		if (pfnCreateWin32SurfaceKHR(instance, &createInfo, allocator, &outSurface) != VK_SUCCESS)
		{
			return false;
		}

		return true;
#else
		return false;
#endif // LEVIATHAN_BUILD_PLATFORM_WIN32
	}

	void DestroyVulkanSurface(VkInstance const instance, VkSurfaceKHR const surface, VkAllocationCallbacks* const allocator)
	{
		vkDestroySurfaceKHR(instance, surface, allocator);
	}

	bool CreateVulkanSwapchain(VkSurfaceKHR surface,
		VkPhysicalDevice physicalDevice,
		VkColorSpaceKHR colorSpace,
		VkFormat format,
		VkPresentModeKHR presentMode,
		VkSwapchainKHR oldSwapchain,
		unsigned int imageCount,
		VkDevice device,
		VkAllocationCallbacks* const allocator,
		VkSwapchainKHR& outSwapchain,
		VkExtent2D& outExtent,
		VkFormat& outFormat)
	{
		// Get surface capabilities.
		VkSurfaceCapabilitiesKHR surfaceCapabilities = {};

		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities) != VK_SUCCESS)
		{
			return false;
		}

		// Get surface format count then formats.
		uint32_t formatCount = 0;

		if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr) != VK_SUCCESS)
		{
			return false;
		}

		std::vector<VkSurfaceFormatKHR> formats(static_cast<size_t>(formatCount));

		if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data()) != VK_SUCCESS)
		{
			return false;
		}

		// Select surface format.
		unsigned int formatIndex = std::numeric_limits<unsigned int>::max();

		for (unsigned int i = 0; i < formatCount; ++i)
		{
			if ((formats[static_cast<size_t>(i)].colorSpace == colorSpace) &&
				(formats[static_cast<size_t>(i)].format == format))
			{
				formatIndex = i;
				break;
			}
		}

		if (formatIndex == std::numeric_limits<unsigned int>::max())
		{
			return false;
		}

		// Get present mode count then present modes.
		uint32_t presentModeCount = 0;

		if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr) != VK_SUCCESS)
		{
			return false;
		}

		std::vector<VkPresentModeKHR> presentModes(presentModeCount);

		if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()) != VK_SUCCESS)
		{
			return false;
		}

		// Select present mode.
		unsigned int presentModeIndex = std::numeric_limits<unsigned int>::max();

		for (unsigned int i = 0; i < presentModeCount; ++i)
		{
			if (presentModes[static_cast<size_t>(i)] == presentMode)
			{
				presentModeIndex = i;
				break;
			}
		}

		if (presentModeIndex == std::numeric_limits<unsigned int>::max())
		{
			return false;
		}

		// Fill out swap chain create info.
		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.surface = surface;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.oldSwapchain = oldSwapchain;
		swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
		swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
		swapchainCreateInfo.minImageCount = std::clamp(imageCount,
			surfaceCapabilities.minImageCount,
			((surfaceCapabilities.maxImageCount != 0) ? surfaceCapabilities.maxImageCount : std::numeric_limits<unsigned int>::max()));
		swapchainCreateInfo.imageFormat = formats[static_cast<size_t>(formatIndex)].format;
		swapchainCreateInfo.imageColorSpace = formats[static_cast<size_t>(formatIndex)].colorSpace;
		swapchainCreateInfo.presentMode = presentModes[static_cast<size_t>(presentModeIndex)];

		// Set output swapchain extent and format.
		outExtent = swapchainCreateInfo.imageExtent;
		outFormat = swapchainCreateInfo.imageFormat;
		
		// Create swap chain.
		return (vkCreateSwapchainKHR(device, &swapchainCreateInfo, allocator, &outSwapchain) == VK_SUCCESS);
	}

	bool RetreiveSwapchainImages(VkDevice device, VkSwapchainKHR swapchain, std::vector<VkImage>& outImages)
	{
		// Get swapchain image count then swapchain images.
		uint32_t imageCount = 0;

		if (vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr) != VK_SUCCESS)
		{
			return false;
		}

		outImages.resize(static_cast<size_t>(imageCount), VK_NULL_HANDLE);

		return vkGetSwapchainImagesKHR(device, swapchain, &imageCount, outImages.data()) == VK_SUCCESS;
	}

	bool CreateVulkanImageView(VkDevice device, 
		VkImage image, 
		const VkImageViewType viewType,
		const VkFormat format,
		const VkImageAspectFlags aspectMask,
		const VkComponentMapping& components,
		VkAllocationCallbacks* const allocator,
		VkImageView& outImageView)
	{
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = image;
		imageViewCreateInfo.viewType = viewType;
		imageViewCreateInfo.format = format;
		imageViewCreateInfo.components = components;
		imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		return (vkCreateImageView(device, &imageViewCreateInfo, allocator, &outImageView) == VK_SUCCESS);
	}

	void DestroyVulkanSwapchain(VkDevice device, VkSwapchainKHR swapchain, VkAllocationCallbacks* const allocator)
	{
		vkDestroySwapchainKHR(device, swapchain, allocator);
	}

	void DestroyVulkanImageView(VkDevice device, VkImageView imageView, VkAllocationCallbacks* const allocator)
	{
		vkDestroyImageView(device, imageView, allocator);
	}
}