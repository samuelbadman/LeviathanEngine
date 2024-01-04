#include "VulkanApi.h"
#include "Logging.h"
#include "LeviathanAssert.h"

namespace LeviathanRenderer
{
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
}