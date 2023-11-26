#pragma once

#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
#define LEVIATHAN_ENTRY_POINT int WINAPI wWinMain([[maybe_unused]] _In_ HINSTANCE hInstance, [[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance, [[maybe_unused]] _In_ PWSTR pCmdLine, [[maybe_unused]] _In_ int nCmdShow)\
{\
	return static_cast<int>(LeviathanCore::LeviathanEntry());\
}\

#else
unsupported platform
#endif

namespace LeviathanCore
{
	unsigned char LeviathanEntry();
}