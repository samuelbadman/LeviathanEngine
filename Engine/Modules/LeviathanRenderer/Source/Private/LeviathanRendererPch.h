#pragma once

// Standard library.
#include <string>
#include <vector>
#include <cassert>
#include <optional>
#include <array>

#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
// Windows.
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#ifdef LEVIATHAN_BUILD_RENDERER_API_VULKAN
// Vulkan.
#include <vulkan/vulkan.h>
#elif defined LEVIATHAN_BUILD_RENDERER_API_DIRECT3D12_PC
// Direct3D12 PC.

#endif

