#pragma once

#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
#define LEVIATHAN_ENTRY_PROTOTYPE int WINAPI wWinMain([[maybe_unused]] _In_ HINSTANCE hInstance, [[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance, [[maybe_unused]] _In_ PWSTR pCmdLine, [[maybe_unused]] _In_ int nCmdShow)
#else
unsupported platform
#endif

namespace LeviathanCore
{
	namespace Platform
	{
		bool CreateDebugConsole();
		bool DestroyDebugConsole();
	}
}