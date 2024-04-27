#pragma once

// Standard library.
#include <string>
#include <cassert>
#include <array>
#include <unordered_map>

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

#endif // LEVIATHAN_BUILD_PLATFORM_WIN32.

#ifdef LEVIATHAN_BUILD_RENDERER_API_DIRECT3D11_PC
// Direct3D11 PC.
#include <d3d11.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3dcompiler.h>

#elif defined LEVIATHAN_BUILD_RENDERER_API_DIRECT3D12_PC
// Direct3D12 PC.
#include <dxcapi.h>

#elif defined LEVIATHAN_BUILD_RENDERER_API_VULKAN
// Vulkan.
#include <vulkan/vulkan.h>

#endif // LEVIATHAN_BUILD_RENDERER_API_DIRECT3D11_PC.

#ifdef LEVIATHAN_WITH_TOOLS

// Direct3D 11 PC.
#ifdef LEVIATHAN_BUILD_RENDERER_API_DIRECT3D11_PC

#include "../../../LeviathanTools/Source/Private/Imgui/imgui_impl_dx11.h"

// Direct3D 12 PC.
#elif defined LEVIATHAN_BUILD_RENDERER_API_DIRECT3D12_PC

#include "../../../LeviathanTools/Source/Private/Imgui/imgui_impl_dx12.h"

// Vulkan.
#elif defined LEVIATHAN_BUILD_RENDERER_API_VULKAN

#include "../../../LeviathanTools/Source/Private/Imgui/imgui_impl_vulkan.h"

#endif // LEVIATHAN_BUILD_RENDERER_API_DIRECT3D11_PC.

#endif // LEVIATHAN_WITH_TOOLS.