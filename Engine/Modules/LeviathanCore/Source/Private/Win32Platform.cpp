#include "Platform.h"
#include "Win32Window.h"

namespace LeviathanCore
{
	namespace Platform
	{
		static constexpr USHORT RawInputDeviceMouseUsagePage = 0x01;
		static constexpr USHORT RawInputDeviceMouseUsage = 0x02;
		static constexpr USHORT RawInputDeviceGameControllerUsagePage = 0x01;
		static constexpr USHORT RawInputDeviceGameControllerUsage = 0x05;
		static constexpr DWORD RawInputDeviceGameControllerFlags = RIDEV_DEVNOTIFY; // Generate WM_INPUT_DEVICE_CHANGED messages in the WndProc function when this device is added/removed.

		static LARGE_INTEGER TicksPerSecond = {};
		static LARGE_INTEGER LastTickCount = {};
		static unsigned long long ElapsedMicroseconds = 0;
		static Callback<GameControllerConnectionEventCallbackType> GameControllerConnectionEventCallback;
		static std::unique_ptr<LeviathanCore::Platform::Window::PlatformWindow> MessageWindow = {};

		// Callback function registered to the message window's game controller connected and disconnected callbacks. This will be called every time a game controller device
		// is connected and disconnected from the platform device.
		static void OnGameControllerConnectionEvent()
		{
			GameControllerConnectionEventCallback.Call();
		}

		// Initializes timing variables. Returns false if the function fails otherwise will return true.
		static bool InitializeTiming()
		{
			if (!QueryPerformanceFrequency(&TicksPerSecond))
			{
				return false;
			}

			if (!QueryPerformanceCounter(&LastTickCount))
			{
				return false;
			}

			return true;
		}

		// Creates the hidden message window that will be used to receive raw input device messages.
		static bool CreateMessageWindow()
		{
			MessageWindow = std::make_unique<LeviathanCore::Platform::Window::PlatformWindow>();
			return MessageWindow->Initialize("Win32MessageWindowClass", "", 0, 0, 0, 0, 0, nullptr, true);
		}

		static bool DestroyMessageWindow()
		{
			if (!DestroyWindow(MessageWindow->GetHWnd()))
			{
				return false;
			}

			if (!MessageWindow->UnregisterWindowClassName())
			{
				return false;
			}

			MessageWindow.reset();

			return true;
		}

		// Registers functions to message window callbacks.
		static void RegisterMessageWindowCallbacks()
		{
			MessageWindow->GetGameControllerConnectedCallback().Register(&OnGameControllerConnectionEvent);
			MessageWindow->GetGameControllerDisconnectedCallback().Register(&OnGameControllerConnectionEvent);
		}

		// Deregisters function to message window callbacks.
		static void DeregisterMessageWindowCallbacks()
		{
			MessageWindow->GetGameControllerConnectedCallback().Deregister(&OnGameControllerConnectionEvent);
			MessageWindow->GetGameControllerDisconnectedCallback().Deregister(&OnGameControllerConnectionEvent);
		}

		// Registers devices that generate raw input messages. Takes the handle to the window to send raw input device messages
		// to, that require a specific window to be sent to. Only this window will receive these messages. Returns true if the 
		// function succeeds otherwise, returns false if the function fails. Call GetLastError for more information.
		static bool RegisterRawInputDevicesWin32(HWND windowHandle)
		{
			RAWINPUTDEVICE rawInputDevices[] =
			{
				RAWINPUTDEVICE
				{
					.usUsagePage = RawInputDeviceMouseUsagePage,
					.usUsage = RawInputDeviceMouseUsage,
					.dwFlags = 0,
					.hwndTarget = nullptr // Null hwnd to have generated raw input messages be sent to the window that currently has keyboard focus.
				},

				RAWINPUTDEVICE
				{
					.usUsagePage = RawInputDeviceGameControllerUsagePage,
					.usUsage = RawInputDeviceGameControllerUsage,
					.dwFlags = RawInputDeviceGameControllerFlags,
					.hwndTarget = windowHandle // Game controller messages require a window to send messages to.
				}
			};

			return RegisterRawInputDevices(rawInputDevices, 2, sizeof(RAWINPUTDEVICE));
		}

		static bool UpdateDeltaTime()
		{
			LARGE_INTEGER currentTickCount = {};
			if (QueryPerformanceCounter(&currentTickCount))
			{
				const uint64_t elapsedTicks = currentTickCount.QuadPart - LastTickCount.QuadPart;

				// Convert to microseconds to not lose precision, by dividing a small number by a large one.
				ElapsedMicroseconds = (elapsedTicks * static_cast<unsigned long long>(1e6)) / TicksPerSecond.QuadPart;

				LastTickCount = currentTickCount;

				return true;
			}

			return false;
		}

		// Dispatches system messages on the message queue to be processed.
		static void DispatchSystemMessages()
		{
			MSG msg = {};
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		bool CreateDebugConsole()
		{
			// Allocate console.
			if (!AllocConsole())
			{
				// AllocConsole failed.
				return false;
			}

			// Redirect standard IO.
			FILE* file;
			if (!(freopen_s(&file, "CONOUT$", "w", stdout) != 0))
			{
				setvbuf(stdout, nullptr, _IONBF, 0);
			}

			if (!(freopen_s(&file, "CONIN$", "r", stdin) != 0))
			{
				setvbuf(stdin, nullptr, _IONBF, 0);
			}

			if (!(freopen_s(&file, "CONOUT$", "w", stderr) != 0))
			{
				setvbuf(stderr, nullptr, _IONBF, 0);
			}

			std::ios::sync_with_stdio(true);
			std::wcout.clear();
			std::cout.clear();
			std::wcerr.clear();
			std::cerr.clear();
			std::wcin.clear();
			std::cin.clear();

			// Console creation succeeded.
			return true;
		}

		bool DestroyDebugConsole()
		{
			// Detach from console.
			if (!FreeConsole())
			{
				// FreeConsole failed.
				return false;
			}

			// Redirect standard IO.
			FILE* file;
			if (!(freopen_s(&file, "NUL:", "r", stdin) != 0))
			{
				setvbuf(stdin, nullptr, _IONBF, 0);
			}

			if (!(freopen_s(&file, "NUL:", "w", stdout) != 0))
			{
				setvbuf(stdout, nullptr, _IONBF, 0);
			}

			if (!(freopen_s(&file, "NUL:", "w", stderr) != 0))
			{
				setvbuf(stderr, nullptr, _IONBF, 0);
			}

			std::ios::sync_with_stdio(false);

			// Console destruction succeeded.
			return true;
		}

		bool Initialize()
		{
			if (!InitializeTiming())
			{
				return false;
			}

			if (!CreateMessageWindow())
			{
				return false;
			}

			RegisterMessageWindowCallbacks();

			if (!RegisterRawInputDevicesWin32(MessageWindow->GetHWnd()))
			{
				return false;
			}

			return true;
		}

		bool Shutdown()
		{
			DeregisterMessageWindowCallbacks();

			if (!DestroyMessageWindow())
			{
				return false;
			}

			return true;
		}

		bool TickPlatform()
		{
			if (!UpdateDeltaTime())
			{
				return false;
			}

			DispatchSystemMessages();

			return true;
		}

		float GetDeltaTimeInMilliseconds()
		{
			return static_cast<float>(ElapsedMicroseconds) * 1e-3f;
		}

		float GetDeltaTimeInSeconds()
		{
			return GetDeltaTimeInMilliseconds() * 1e-3f;
		}

		Callback<GameControllerConnectionEventCallbackType>& GetGameControllerConnectionEventCallback()
		{
			return GameControllerConnectionEventCallback;
		}

		namespace Displays
		{
			int GetDisplayCount()
			{
				return GetSystemMetrics(SM_CMONITORS);
			}

			DisplayDetails GetDisplayDetails(const int displayIndex)
			{
				// Check the index is valid.
				const int connectedDisplaysCount = GetDisplayCount();
				if ((connectedDisplaysCount == 0) || (displayIndex >= connectedDisplaysCount))
				{
					return DisplayDetails{};
				}

				// Initialize the DISPLAY_DEVICE structure.
				DISPLAY_DEVICE displayDevice = {};
				displayDevice.cb = sizeof(DISPLAY_DEVICE);

				// Enumerate display devices
				EnumDisplayDevices(NULL, displayIndex, &displayDevice, EDD_GET_DEVICE_INTERFACE_NAME);

				// Enumerate the display device's settings.
				DEVMODEA displaySettings = {};
				EnumDisplaySettings(displayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &displaySettings);

				// Fill in and return display details with the device's details.
				DisplayDetails details = {};
				details.TopLeftX = displaySettings.dmPosition.x;
				details.TopLeftY = displaySettings.dmPosition.y;
				details.Width = displaySettings.dmPelsWidth;
				details.Height = displaySettings.dmPelsHeight;
				details.VerticalRefreshRateHertz = displaySettings.dmDisplayFrequency;
				details.Name = displayDevice.DeviceName;
				details.AdapterName = displayDevice.DeviceString;

				return details;
			}
		}

		void ShowPlatformCursor(const bool show)
		{
			ShowCursor(show);
		}

		bool IsPlatformCursorVisible()
		{
			CURSORINFO cursorInfo = { sizeof(CURSORINFO) };

			if (!GetCursorInfo(&cursorInfo))
			{
				// GetCursorInfo() failed.
				return false;
			}

			if (cursorInfo.flags == 0)
			{
				// Cursor is hidden.
				return false;
			}
			else if (cursorInfo.flags == 1)
			{
				// Cursor is visible.
				return true;
			}
			else if (cursorInfo.flags == 2)
			{
				// TODO: Implement suppressed response.
				// Cursor is suppressed.
				return false;
			}

			// Error. This code shouldn't be reachable.
			return false;
		}

		bool ReleasePlatformCursor()
		{
			return ClipCursor(nullptr);
		}
	}
}