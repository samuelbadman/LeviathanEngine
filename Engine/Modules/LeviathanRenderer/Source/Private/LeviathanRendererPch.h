#pragma once

// Standard library.
#include <string>
#include <vector>
#include <cassert>
#include <optional>
#include <array>
#include <algorithm>
#include <chrono>

#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
// Windows.
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef max
#undef max
#endif // max.

#ifdef min
#undef min
#endif // min.

#endif

#ifdef LEVIATHAN_BUILD_RENDERER_API_DIRECT3D12_PC
// Direct3D12 PC.

#elif defined LEVIATHAN_BUILD_RENDERER_API_VULKAN
// Vulkan.
#include <vulkan/vulkan.h>

#endif

