#include "PlatformWindow.h"
#include "Win32Window.h"

namespace LeviathanCore
{
	namespace Platform
	{
		namespace Window
		{
			static constexpr DWORD WindowModeWindowedStyle = WS_OVERLAPPEDWINDOW;
			static constexpr DWORD WindowModeBorderlessStyle = WS_POPUPWINDOW;
			static constexpr DWORD WindowModeWindowed_NoResizeStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
			static constexpr DWORD WindowModeWindowed_NoDragSizeStyle = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME;

			static DWORD constexpr GetModeFlags(const PlatformWindowMode mode)
			{
				switch (mode)
				{
				case PlatformWindowMode::Windowed: return WindowModeWindowedStyle;
				case PlatformWindowMode::Windowed_NoResize: return WindowModeWindowed_NoResizeStyle;
				case PlatformWindowMode::Windowed_NoDragSize: return WindowModeWindowed_NoDragSizeStyle;
				case PlatformWindowMode::Borderless: return WindowModeBorderlessStyle;
				default: return 0;
				}
			}

			// Start of exposed platform window interface implementation.
			// --------------------------------------------------------
			PlatformWindow* CreatePlatformWindow()
			{
				return new PlatformWindow();
			}

			void DestroyPlatformWindow(PlatformWindow* const platformWindow)
			{
				delete platformWindow;
			}

			bool InitializePlatformWindow(PlatformWindow* const platformWindow, const PlatformWindowDescription& description)
			{
				return platformWindow->Initialize(description.UniqueName, description.Title, GetModeFlags(description.Mode), description.PositionX, description.PositionY,
					description.Width, description.Height, static_cast<HWND>(description.ParentWindowPlatformHandle), false);
			}

			bool ShutdownPlatformWindow(PlatformWindow* const platformWindow)
			{
				if (!DestroyWindow(platformWindow->GetHWnd()))
				{
					return false;
				}

				if (!platformWindow->UnregisterWindowClassName())
				{
					return false;
				}

				platformWindow->Reset();

				return true;
			}

			void* GetPlatformWindowPlatformHandle(PlatformWindow* const platformWindow)
			{
				return static_cast<void*>(platformWindow->GetHWnd());
			}

			Callback<PlatformWindowDestroyedCallbackType>& GetPlatformWindowDestroyedCallback(PlatformWindow* const platformWindow)
			{
				return platformWindow->GetDestroyedCallback();
			}

			Callback<PlatformWindowClosedCallbackType>& GetPlatformWindowClosedCallback(PlatformWindow* const platformWindow)
			{
				return platformWindow->GetClosedCallback();
			}

			Callback<PlatformWindowLostFocusCallbackType>& GetPlatformWindowLostFocusCallback(PlatformWindow* const platformWindow)
			{
				return platformWindow->GetLostFocusCallback();
			}

			Callback<PlatformWindowReceivedFocusCallbackType>& GetPlatformWindowReceivedFocusCallback(PlatformWindow* const platformWindow)
			{
				return platformWindow->GetReceivedFocusCallback();
			}

			Callback<PlatformWindowResizedCallbackType>& GetPlatformWindowResizedCallback(PlatformWindow* const platformWindow)
			{
				return platformWindow->GetResizedCallback();
			}

			Callback<PlatformWindowExitSizeMoveCallbackType>& GetPlatformWindowExitSizeMoveCallback(PlatformWindow* const platformWindow)
			{
				return platformWindow->GetExitSizeMoveCallback();
			}

			Callback<PlatformWindowEnterSizeMoveCallbackType>& GetPlatformWindowEnterSizeMoveCallback(PlatformWindow* const platformWindow)
			{
				return platformWindow->GetEnterSizeMoveCallback();
			}

			Callback<PlatformWindowRestoredCallbackType>& GetPlatformWindowRestoredCallback(PlatformWindow* const platformWindow)
			{
				return platformWindow->GetRestoredCallback();
			}

			Callback<PlatformWindowMinimizedCallbackType>& GetPlatformWindowMinimizedCallback(PlatformWindow* const platformWindow)
			{
				return platformWindow->GetMinimizedCallback();
			}

			Callback<PlatformWindowMaximizedCallbackType>& GetPlatformWindowMaximizedCallback(PlatformWindow* const platformWindow)
			{
				return platformWindow->GetMaximizedCallback();
			}

			Callback<PlatformWindowKeyboardInputCallbackType>& GetPlatformWindowKeyboardInputCallback(PlatformWindow* const platformWindow)
			{
				return platformWindow->GetKeyboardInputCallback();
			}

			Callback<PlatformWindowMouseInputCallbackType>& GetPlatformWindowMouseInputCallback(PlatformWindow* const platformWindow)
			{
				return platformWindow->GetMouseInputCallback();
			}
			// --------------------------------------------------------
			// End of exposed platform window interface implementation.

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

			// Returns the input key from a wparam.
			static InputKey::Keys constexpr GetKeyFromWParam(const WPARAM wParam)
			{
				return static_cast<InputKey::Keys>(static_cast<std::underlying_type<InputKey::Keys>::type>(wParam));
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
					((messageWindow) ? ((hWndParent == nullptr) ? HWND_MESSAGE : hWndParent) : hWndParent),
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

				GetClientAreaDimensions(ClientAreaWidth, ClientAreaHeight);

				return true;
			}

			bool PlatformWindow::UnregisterWindowClassName()
			{
				return (UnregisterClassA(ClassName.c_str(), nullptr));
			}

			LRESULT PlatformWindow::WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
			{
				// Handle message.
				switch (msg)
				{
				case WM_MOUSEWHEEL:
				{
					const int16_t delta = GET_WHEEL_DELTA_WPARAM(wParam);

					if (delta > 0)
					{
						// Mouse wheel up.
						WndProcMouseInput(InputKey::Keys::MouseWheelUpAxis, 1.0f);
					}
					else if (delta < 0)
					{
						// Mouse wheel down.
						WndProcMouseInput(InputKey::Keys::MouseWheelDownAxis, 1.0f);
					}

					return 0;
				}

				case WM_INPUT:
				{
					UINT dataSize = 0;
					GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &dataSize, sizeof(RAWINPUTHEADER));
					if (dataSize == 0)
					{
						return 0;
					}

					std::unique_ptr<BYTE[]> rawData = std::make_unique<BYTE[]>(dataSize);
					if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawData.get(), &dataSize, sizeof(RAWINPUTHEADER)) != dataSize)
					{
						return 0;
					}

					const RAWINPUT* const rawInput = reinterpret_cast<const RAWINPUT*>(rawData.get());
					if (rawInput->header.dwType != RIM_TYPEMOUSE)
					{
						return 0;
					}

					// Raw mouse delta.
					WndProcMouseInput(InputKey::Keys::MouseXAxis, static_cast<float>(rawInput->data.mouse.lLastX));
					WndProcMouseInput(InputKey::Keys::MouseYAxis, static_cast<float>(rawInput->data.mouse.lLastY));
					return 0;
				}

				case WM_LBUTTONDOWN:
				{
					// Input.
					WndProcMouseInput(InputKey::Keys::LeftMouseButton, 1.0f);
					return 0;
				}

				case WM_RBUTTONDOWN:
				{
					// Input.
					WndProcMouseInput(InputKey::Keys::RightMouseButton, 1.0f);
					return 0;
				}

				case WM_MBUTTONDOWN:
				{
					// Input.
					WndProcMouseInput(InputKey::Keys::MiddleMouseButton, 1.0f);
					return 0;
				}

				case WM_LBUTTONUP:
				{
					// Input.
					WndProcMouseInput(InputKey::Keys::LeftMouseButton, 0.0f);
					return 0;
				}

				case WM_RBUTTONUP:
				{
					// Input.
					WndProcMouseInput(InputKey::Keys::RightMouseButton, 0.0f);
					return 0;
				}

				case WM_MBUTTONUP:
				{
					// Input.
					WndProcMouseInput(InputKey::Keys::MiddleMouseButton, 0.0f);
					return 0;
				}

				case WM_SYSKEYDOWN:
				case WM_KEYDOWN:
				{
					// Handle alt+f4 exit shortcut.
					bool AltBit = false;
					AltBit = (lParam & (1 << 29)) != 0;
					if (AltBit && (static_cast<int16_t>(wParam) == VK_F4))
					{
						SendMessage(hWnd, WM_CLOSE, 0, 0);
						return 0;
					}
					// Input.
					WndProcKeyboardInput(GetKeyFromWParam(wParam), static_cast<bool>(lParam & 0x40000000), 1.0f);
					return 0;
				}

				case WM_SYSKEYUP:
				case WM_KEYUP:
				{
					// Input.
					WndProcKeyboardInput(GetKeyFromWParam(wParam), false, 0.0f);
					return 0;
				}

				case WM_SIZE:
				{
					const uint32_t resizedWidth = LOWORD(lParam);
					const uint32_t resizedHeight = HIWORD(lParam);

					switch (wParam)
					{
					case SIZE_MAXIMIZED:
					{
						// Maximized.
						WndProcMaximized();
						// Resized.
						WndProcResized(resizedWidth, resizedHeight);
						return 0;
					}

					case SIZE_MINIMIZED:
					{
						// Minimized.
						WndProcMinimized();
						return 0;
					}

					case SIZE_RESTORED:
					{
						// Restored.
						WndProcRestored();
						if (!InSizeMove)
						{
							// Resized.
							WndProcResized(resizedWidth, resizedHeight);
						}
						return 0;
					}
					}

					return 0;
				}

				case WM_ENTERSIZEMOVE:
				{
					// Entered size move.
					InSizeMove = true;
					WndProcEnterSizeMove();
					return 0;
				}

				case WM_EXITSIZEMOVE:
				{
					// Exited size move.
					InSizeMove = false;
					WndProcExitSizeMove();
					// Resized.
					int resizedWidth = 0;
					int resizedHeight = 0;
					GetClientAreaDimensions(resizedWidth, resizedHeight);
					WndProcResized(resizedWidth, resizedHeight);
					return 0;
				}

				case WM_ACTIVATEAPP:
				{
					if (wParam == TRUE)
					{
						// Received focus.
						WndProcReceivedFocus();
						return 0;
					}
					else if (wParam == FALSE)
					{
						// Lost focus.
						WndProcLostFocus();
						return 0;
					}
					return 0;
				}

				case WM_INPUT_DEVICE_CHANGE:
				{
					// WM_INPUT_DEVICE_CHANGE message is only received by the window passed to RegisterRawInputDevices. 
					// This message will only be received by one window in the application. Usually, this will be the main window.
					if (wParam == GIDC_ARRIVAL)
					{
						// Game controller connected.
						WndProcGameControllerConnected();
					}
					else if (wParam == GIDC_REMOVAL)
					{
						// Game controller disconnected.
						WndProcGameControllerDisconnected();
					}
					return 0;
				}

				case WM_CLOSE:
				{
					// Closed.
					WndProcClosed();
					return 0;
				}

				case WM_DESTROY:
				{
					// Destroyed.
					WndProcDestroyed();
					return 0;
				}

				default:
				{
					return DefWindowProcA(hWnd, msg, wParam, lParam);
				}
				}
			}

			bool PlatformWindow::SetTitle(std::string_view newTitle)
			{
				return SetWindowTextA(Handle, newTitle.data());
			}

			bool PlatformWindow::GetClientAreaDimensions(int& outWidth, int& outHeight)
			{
				RECT clientRect = {};

				if (!GetClientRect(Handle, &clientRect))
				{
					// Failed to get client rect.
					outWidth = 0;
					outHeight = 0;
					return false;
				}

				outWidth = static_cast<int>(clientRect.right - clientRect.left);
				outHeight = static_cast<int>(clientRect.bottom - clientRect.top);
				return true;
			}

			void PlatformWindow::Reset()
			{
				Handle = nullptr;
				InSizeMove = false;
				ClassName = {};
			}

			void PlatformWindow::WndProcDestroyed()
			{
				DestroyedCallback.Call();
			}

			void PlatformWindow::WndProcClosed()
			{
				ClosedCallback.Call();
			}

			void PlatformWindow::WndProcGameControllerDisconnected()
			{
				GameControllerDisconnectedCallback.Call();
			}

			void PlatformWindow::WndProcGameControllerConnected()
			{
				GameControllerConnectedCallback.Call();
			}

			void PlatformWindow::WndProcLostFocus()
			{
				LostFocusCallback.Call();
			}

			void PlatformWindow::WndProcReceivedFocus()
			{
				ReceivedFocusCallback.Call();
			}

			void PlatformWindow::WndProcResized(int resizedWidth, int resizedHeight)
			{
				if ((resizedWidth == ClientAreaWidth) && (resizedHeight == ClientAreaHeight))
				{
					return;
				}

				ClientAreaWidth = resizedWidth;
				ClientAreaHeight = resizedHeight;

				ResizedCallback.Call(resizedWidth, resizedHeight);
			}

			void PlatformWindow::WndProcExitSizeMove()
			{
				ExitSizeMoveCallback.Call();
			}

			void PlatformWindow::WndProcEnterSizeMove()
			{
				EnterSizeMoveCallback.Call();
			}

			void PlatformWindow::WndProcRestored()
			{
				RestoredCallback.Call();
			}

			void PlatformWindow::WndProcMinimized()
			{
				MinimizedCallback.Call();
			}

			void PlatformWindow::WndProcMaximized()
			{
				MaximizedCallback.Call();
			}

			void PlatformWindow::WndProcKeyboardInput(const InputKey::Keys key, const bool repeatedKey, const float data)
			{
				KeyboardInputCallback.Call(key, repeatedKey, data);
			}

			void PlatformWindow::WndProcMouseInput(const InputKey::Keys key, const float data)
			{
				MouseInputCallback.Call(key, data);
			}
		}
	}
}