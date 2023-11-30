#include "Platform/Win32/Win32Window.h"
#include "Platform/Window.h"

static std::vector<std::unique_ptr<LeviathanCore::Platform::Window>> AllocatedWindows = {};

LeviathanCore::Platform::WindowHandle LeviathanCore::Platform::AllocateWindow()
{
	AllocatedWindows.emplace_back(std::make_unique<LeviathanCore::Platform::Window>());
	return (AllocatedWindows.size() - 1);
}

void LeviathanCore::Platform::FreeWindow(const LeviathanCore::Platform::WindowHandle Window)
{
	AllocatedWindows.erase(AllocatedWindows.begin() + Window);
}

