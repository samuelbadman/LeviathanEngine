#pragma once

// Standard library.
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <type_traits>
#include <array>

#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
// Win32.
//#define NO_MIN_MAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

// XInput.
#include <xinput.h>

#undef min
#undef max
#endif // LEVIATHAN_BUILD_PLATFORM_WIN32