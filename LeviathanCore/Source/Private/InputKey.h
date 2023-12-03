#pragma once

namespace LeviathanCore
{
	// Class containing input key platform definitions, helper functions and a key value.
	class InputKey
	{
	public:
		enum class Keys : unsigned short
		{
#ifdef LEVIATHAN_BUILD_PLATFORM_WIN32
			// Keyboard keys.
			Backspace = VK_BACK,
			Tab = VK_TAB,
			Enter = VK_RETURN,
			LeftShift = VK_LSHIFT,
			RightShift = VK_RSHIFT,
			CapsLock = VK_CAPITAL,
			Escape = VK_ESCAPE,
			SpaceBar = VK_SPACE,
			PageUp = VK_PRIOR,
			PageDown = VK_NEXT,
			End = VK_END,
			Home = VK_HOME,
			Insert = VK_INSERT,
			Delete = VK_DELETE,
			Left = VK_LEFT,
			Right = VK_RIGHT,
			Up = VK_UP,
			Down = VK_DOWN,
			Zero = 0x30,
			One = 0x31,
			Two = 0x32,
			Three = 0x33,
			Four = 0x34,
			Five = 0x35,
			Six = 0x36,
			Seven = 0x37,
			Eight = 0x38,
			Nine = 0x39,
			A = 0x41,
			B = 0x42,
			C = 0x43,
			D = 0x44,
			E = 0x45,
			F = 0x46,
			G = 0x47,
			H = 0x48,
			I = 0x49,
			J = 0x4A,
			K = 0x4B,
			L = 0x4C,
			M = 0x4D,
			N = 0x4E,
			O = 0x4F,
			P = 0x50,
			Q = 0x51,
			R = 0x52,
			S = 0x53,
			T = 0x54,
			U = 0x55,
			V = 0x56,
			W = 0x57,
			X = 0x58,
			Y = 0x59,
			Z = 0x5A,
			Numpad0 = VK_NUMPAD0,
			Numpad1 = VK_NUMPAD1,
			Numpad2 = VK_NUMPAD2,
			Numpad3 = VK_NUMPAD3,
			Numpad4 = VK_NUMPAD4,
			Numpad5 = VK_NUMPAD5,
			Numpad6 = VK_NUMPAD6,
			Numpad7 = VK_NUMPAD7,
			Numpad8 = VK_NUMPAD8,
			Numpad9 = VK_NUMPAD9,
			F1 = VK_F1,
			F2 = VK_F2,
			F3 = VK_F3,
			F4 = VK_F4,
			F5 = VK_F5,
			F6 = VK_F6,
			F7 = VK_F7,
			F8 = VK_F8,
			F9 = VK_F9,
			F10 = VK_F10,
			F11 = VK_F11,
			F12 = VK_F12,
			LeftCtrl = VK_LCONTROL,
			RightCtrl = VK_RCONTROL,
			Alt = VK_MENU,

			// Mouse keys.
			LeftMouseButton = MK_LBUTTON,
			RightMouseButton = MK_RBUTTON,
			MiddleMouseButton = MK_MBUTTON,
			MouseWheelUpAxis = 399,
			MouseWheelDownAxis = 400,
			MouseXAxis = 401,
			MouseYAxis = 402,

			// Gamepad keys.
			GamepadFaceButtonBottom = XINPUT_GAMEPAD_A,
			GamepadFaceButtonRight = XINPUT_GAMEPAD_B,
			GamepadFaceButtonLeft = XINPUT_GAMEPAD_X,
			GamepadFaceButtonTop = XINPUT_GAMEPAD_Y,
			GamepadDPadUp = XINPUT_GAMEPAD_DPAD_UP,
			GamepadDPadDown = XINPUT_GAMEPAD_DPAD_DOWN,
			GamepadDPadLeft = XINPUT_GAMEPAD_DPAD_LEFT,
			GamepadDPadRight = XINPUT_GAMEPAD_DPAD_RIGHT,
			GamepadLeftThumbstickButton = XINPUT_GAMEPAD_LEFT_THUMB,
			GamepadRightThumbstickButton = XINPUT_GAMEPAD_RIGHT_THUMB,
			GamepadSpecialLeft = XINPUT_GAMEPAD_BACK,
			GamepadSpecialRight = XINPUT_GAMEPAD_START,
			GamepadLeftShoulder = XINPUT_GAMEPAD_LEFT_SHOULDER,
			GamepadRightShoulder = XINPUT_GAMEPAD_RIGHT_SHOULDER,
			GamepadLeftThumbstickXAxis = 404,
			GamepadLeftThumbstickYAxis = 405,
			GamepadRightThumbstickXAxis = 406,
			GamepadRightThumbstickYAxis = 407,
			GamepadLeftThumbstickUp = 408,
			GamepadLeftThumbstickDown = 409,
			GamepadLeftThumbstickLeft = 410,
			GamepadLeftThumbstickRight = 411,
			GamepadRightThumbstickUp = 412,
			GamepadRightThumbstickDown = 413,
			GamepadRightThumbstickLeft = 414,
			GamepadRightThumbstickRight = 415,
			GamepadLeftTrigger = 416,
			GamepadRightTrigger = 417,
			GamepadLeftTriggerAxis = 418,
			GamepadRightTriggerAxis = 419,

			// Empty .
			Empty = 500
#endif
		};

	private:
		// The key value of this instance.
		Keys KeyValue;

	public:
		// Constructs an input key with a key value.
		InputKey(Keys key = Keys::Empty);

		void SetKey(const Keys key) { KeyValue = key; }
		Keys GetKey() const { return KeyValue; }

		// Returns true if key is a keyboard key otherwise, returns false if key is not a keyboard key. This is the static version of the function.
		static bool constexpr IsKeyboardKey(const Keys key)
		{
			return (key == Keys::Backspace) ||
				(key == Keys::Tab) ||
				(key == Keys::Enter) ||
				(key == Keys::LeftShift) ||
				(key == Keys::RightShift) ||
				(key == Keys::CapsLock) ||
				(key == Keys::Escape) ||
				(key == Keys::SpaceBar) ||
				(key == Keys::PageUp) ||
				(key == Keys::PageDown) ||
				(key == Keys::End) ||
				(key == Keys::Home) ||
				(key == Keys::Insert) ||
				(key == Keys::Delete) ||
				(key == Keys::Left) ||
				(key == Keys::Right) ||
				(key == Keys::Up) ||
				(key == Keys::Down) ||
				(key == Keys::Zero) ||
				(key == Keys::One) ||
				(key == Keys::Two) ||
				(key == Keys::Three) ||
				(key == Keys::Four) ||
				(key == Keys::Five) ||
				(key == Keys::Six) ||
				(key == Keys::Seven) ||
				(key == Keys::Eight) ||
				(key == Keys::Nine) ||
				(key == Keys::A) ||
				(key == Keys::B) ||
				(key == Keys::C) ||
				(key == Keys::D) ||
				(key == Keys::E) ||
				(key == Keys::F) ||
				(key == Keys::G) ||
				(key == Keys::H) ||
				(key == Keys::I) ||
				(key == Keys::J) ||
				(key == Keys::K) ||
				(key == Keys::L) ||
				(key == Keys::M) ||
				(key == Keys::N) ||
				(key == Keys::O) ||
				(key == Keys::P) ||
				(key == Keys::Q) ||
				(key == Keys::R) ||
				(key == Keys::S) ||
				(key == Keys::T) ||
				(key == Keys::U) ||
				(key == Keys::V) ||
				(key == Keys::W) ||
				(key == Keys::X) ||
				(key == Keys::Y) ||
				(key == Keys::Z) ||
				(key == Keys::Numpad0) ||
				(key == Keys::Numpad1) ||
				(key == Keys::Numpad2) ||
				(key == Keys::Numpad3) ||
				(key == Keys::Numpad4) ||
				(key == Keys::Numpad5) ||
				(key == Keys::Numpad6) ||
				(key == Keys::Numpad7) ||
				(key == Keys::Numpad8) ||
				(key == Keys::Numpad9) ||
				(key == Keys::F1) ||
				(key == Keys::F2) ||
				(key == Keys::F3) ||
				(key == Keys::F4) ||
				(key == Keys::F5) ||
				(key == Keys::F6) ||
				(key == Keys::F7) ||
				(key == Keys::F8) ||
				(key == Keys::F9) ||
				(key == Keys::F10) ||
				(key == Keys::F11) ||
				(key == Keys::F12) ||
				(key == Keys::LeftCtrl) ||
				(key == Keys::RightCtrl) ||
				(key == Keys::Alt);
		}

		// Returns true if key is a keyboard key otherwise, returns false if key is not a keyboard key. This is the instance version of the function.
		bool constexpr IsKeyboardKey() const
		{
			return IsKeyboardKey(KeyValue);
		}

		// Returns true if key is a mouse key otherwise, returns false if key is not a mouse key. This is the static version of the function.
		static bool constexpr IsMouseKey(const Keys key)
		{
			return (key == Keys::LeftMouseButton) ||
				(key == Keys::RightMouseButton) ||
				(key == Keys::MiddleMouseButton) ||
				(key == Keys::MouseWheelUpAxis) ||
				(key == Keys::MouseWheelDownAxis) ||
				(key == Keys::MouseXAxis) ||
				(key == Keys::MouseYAxis);
		}

		// Returns true if key is a mouse key otherwise, returns false if key is not a mouse key. This is the instance version of the function.
		bool constexpr IsMouseKey() const
		{
			return IsMouseKey(KeyValue);
		}

		// Returns true if key is a gamepad key otherwise, returns false if key is not a gamepad key. This is the static version of the function.
		static bool constexpr IsGamepadKey(const Keys key)
		{
			return (key == Keys::GamepadFaceButtonBottom) ||
				(key == Keys::GamepadFaceButtonRight) ||
				(key == Keys::GamepadFaceButtonLeft) ||
				(key == Keys::GamepadFaceButtonTop) ||
				(key == Keys::GamepadDPadUp) ||
				(key == Keys::GamepadDPadDown) ||
				(key == Keys::GamepadDPadLeft) ||
				(key == Keys::GamepadDPadRight) ||
				(key == Keys::GamepadLeftThumbstickButton) ||
				(key == Keys::GamepadRightThumbstickButton) ||
				(key == Keys::GamepadSpecialLeft) ||
				(key == Keys::GamepadSpecialRight) ||
				(key == Keys::GamepadLeftShoulder) ||
				(key == Keys::GamepadRightShoulder) ||
				(key == Keys::GamepadLeftThumbstickXAxis) ||
				(key == Keys::GamepadLeftThumbstickYAxis) ||
				(key == Keys::GamepadRightThumbstickXAxis) ||
				(key == Keys::GamepadRightThumbstickYAxis) ||
				(key == Keys::GamepadLeftThumbstickUp) ||
				(key == Keys::GamepadLeftThumbstickDown) ||
				(key == Keys::GamepadLeftThumbstickLeft) ||
				(key == Keys::GamepadLeftThumbstickRight) ||
				(key == Keys::GamepadRightThumbstickUp) ||
				(key == Keys::GamepadRightThumbstickDown) ||
				(key == Keys::GamepadRightThumbstickLeft) ||
				(key == Keys::GamepadRightThumbstickRight) ||
				(key == Keys::GamepadLeftTrigger) ||
				(key == Keys::GamepadRightTrigger) ||
				(key == Keys::GamepadLeftTriggerAxis) ||
				(key == Keys::GamepadRightTriggerAxis);
		}

		// Returns true if key is a gamepad key otherwise, returns false if key is not a gamepad key. This is the instance version of the function.
		bool constexpr IsGamepadKey() const
		{
			return IsGamepadKey(KeyValue);
		}
	};
}