#pragma once

namespace LeviathanCore
{
	namespace Platform
	{
		using WindowHandle = size_t;

		class Window;

		WindowHandle AllocateWindow();
		void FreeWindow(const WindowHandle Window);
	}
}