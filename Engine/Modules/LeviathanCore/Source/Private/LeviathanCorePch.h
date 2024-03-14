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
