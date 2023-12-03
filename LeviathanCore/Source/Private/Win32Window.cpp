#include "PlatformWindow.h"
#include "Win32Window.h"

namespace LeviathanCore
{
	namespace Platform
	{
		// Window procedure callback for every window.
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			// Get window instance and call its wnd proc handler
			PlatformWindow* window = reinterpret_cast<PlatformWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

			if (window == nullptr)
			{
				return 0;
			}

			return window->WndProcHandler(hWnd, msg, wParam, lParam);
		}

		// Initializes the window procedure callback.
		static LRESULT CALLBACK InitWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			switch (msg)
			{
			case WM_NCCREATE:
			{
				// Get the create parameters
				const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);

				// Get the window instance pointer from the create parameters
				PlatformWindow* pWindow = reinterpret_cast<PlatformWindow*>(pCreate->lpCreateParams);

				// Set the window instance pointer
				SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));

				// Set the window procedure pointer
				SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));

				// Call the window procedure
				return WndProc(hWnd, msg, wParam, lParam);
			}
			}
			return 0;
		}

		bool PlatformWindow::Initialize(std::string_view uniqueName, std::string_view windowTitle, DWORD style, int positionX, int positionY, int width, int height, 
			HWND hWndParent, bool messageWindow)
		{
			InSizeMove = false;
			ClassName = uniqueName;

			// Register window class
			WNDCLASSEX wndClass = {};
			wndClass.cbSize = sizeof(WNDCLASSEX);
			wndClass.style = CS_HREDRAW | CS_VREDRAW;
			wndClass.lpfnWndProc = &InitWndProc;
			wndClass.lpszClassName = uniqueName.data();
			wndClass.cbClsExtra = 0;
			wndClass.cbWndExtra = 0;
			wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
			wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wndClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
			wndClass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
			wndClass.lpszMenuName = nullptr;

			const ATOM windowClassId = RegisterClassExA(&wndClass);
			if (windowClassId == 0)
			{
				// Class registration failed.
				return false;
			}

			// Create window
			Handle = CreateWindowExA(
				0,
				uniqueName.data(),
				windowTitle.data(),
				style,
				positionX,
				positionY,
				width,
				height,
				(messageWindow) ? ((hWndParent == nullptr) ? HWND_MESSAGE : hWndParent) : hWndParent,
				nullptr,
				nullptr,
				this
			);

			if (Handle == nullptr)
			{
				return false;
			}

			if (!messageWindow)
			{
				ShowWindow(Handle, SW_SHOW);
			}

			return true;
		}

		bool PlatformWindow::UnregisterWindowClassName()
		{
			return (UnregisterClassA(ClassName.c_str(), nullptr));
		}

		LRESULT PlatformWindow::WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			return DefWindowProcA(hWnd, msg, wParam, lParam);
		}
	}
}