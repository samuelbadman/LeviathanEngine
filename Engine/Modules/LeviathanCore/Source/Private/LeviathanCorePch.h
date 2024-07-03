#pragma once

// Standard library.
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <type_traits>
#include <cassert>
#include <chrono>
#include <array>
#include <fstream>
#include <filesystem>
#include <string>

#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
// Win32.
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

// XInput.
#include <Xinput.h>

#undef min
#undef max
#endif // LEVIATHAN_BUILD_PLATFORM_WIN32

// GLM.
#include "GLM_1.0.1/glm.hpp"
#include "GLM_1.0.1/gtc/matrix_transform.hpp"
#include "GLM_1.0.1/gtc/quaternion.hpp"

#ifdef LEVIATHAN_WITH_TOOLS
// ImGui.
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../../../LeviathanTools/Source/Private/Imgui/imgui.h"

// ImGui Windows.
#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32

#include "../../../LeviathanTools/Source/Private/Imgui/imgui_impl_win32.h"

#endif // LEVIATHAN_BUILD_PLATFORM_WIN32.

#endif // LEVIATHAN_WITH_TOOLS.