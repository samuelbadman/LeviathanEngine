#pragma once

namespace LeviathanCore
{
	namespace Platform
	{
		class PlatformWindow
		{
		private:
			// Windows handle to the window instance.
			HWND Handle = nullptr;

			// Flags if the window is currently in a size move operation.
			bool InSizeMove = false;

			// The unique class name used to register the window class with the Windows platform. Used to unregister the window.
			std::string ClassName = {};

		public:
			bool Initialize(std::string_view uniqueName, std::string_view windowTitle, DWORD style, int positionX, 
				int positionY, int width, int height, HWND hWndParent, bool messageWindow);
			bool UnregisterWindowClassName();
			LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
			HWND GetHWnd() const { return Handle; }
		};
	}
}