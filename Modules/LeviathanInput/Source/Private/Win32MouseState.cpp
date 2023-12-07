#include "Win32MouseState.h"
#include "Bits.h"
#include "Win32Input.h"

namespace LeviathanInput
{
	namespace PlatformInput
	{
		// Returns the bit representing the key in the mouse state.
		static constexpr std::underlying_type<Win32MouseState::MouseButtonKeyBits>::type GetMouseButtonKeyBit(const LeviathanCore::InputKey::Keys key)
		{
			switch (key)
			{
			case LeviathanCore::InputKey::Keys::LeftMouseButton: return static_cast<std::underlying_type<Win32MouseState::MouseButtonKeyBits>::type>(Win32MouseState::MouseButtonKeyBits::LeftMouseButtonBit);
			case LeviathanCore::InputKey::Keys::RightMouseButton: return static_cast<std::underlying_type<Win32MouseState::MouseButtonKeyBits>::type>(Win32MouseState::MouseButtonKeyBits::RightMouseButtonBit);
			case LeviathanCore::InputKey::Keys::MiddleMouseButton: return static_cast<std::underlying_type<Win32MouseState::MouseButtonKeyBits>::type>(Win32MouseState::MouseButtonKeyBits::MiddleMouseButtonBit);
			case LeviathanCore::InputKey::Keys::MouseWheelUpAxis: return static_cast<std::underlying_type<Win32MouseState::MouseButtonKeyBits>::type>(Win32MouseState::MouseButtonKeyBits::MouseWheelUpAxisBit);
			case LeviathanCore::InputKey::Keys::MouseWheelDownAxis: return static_cast<std::underlying_type<Win32MouseState::MouseButtonKeyBits>::type>(Win32MouseState::MouseButtonKeyBits::MouseWheelDownAxisBit);
			default: return static_cast<std::underlying_type<Win32MouseState::MouseButtonKeyBits>::type>(Win32MouseState::MouseButtonKeyBits::MaxBit);
			}
		}

		void Win32MouseState::UpdateMouseWheel(const LeviathanCore::InputKey::Keys wheelKey, const bool setBit)
		{
			if (!IsWheelKey(wheelKey))
			{
				return;
			}

			const uint8_t wheelBit = GetMouseButtonKeyBit(wheelKey);
			if (!IsValidBit(wheelBit))
			{
				return;
			}

			if (setBit)
			{
				SET_BIT(buttonState, wheelBit);
			}
			else
			{
				CLEAR_BIT(buttonState, wheelBit);
			}
		}

		void Win32MouseState::UpdatePosition(const long long x, const long long y)
		{
			position[0] = x;
			position[1] = y;
		}

		void Win32MouseState::UpdateButtonState(const LeviathanCore::InputKey::Keys key)
		{
			if (!IsButtonKey(key))
			{
				return;
			}

			if (PlatformInput::IsKeyDownAsyncKeyState(key))
			{
				// Key down.
				SetButtonKeyBit(key);
			}
			else
			{
				ClearButtonKeyBit(key);
			}
		}

		bool Win32MouseState::IsButtonPressed(const LeviathanCore::InputKey::Keys key) const
		{
			if (!IsButtonKey(key))
			{
				return false;
			}

			const unsigned char buttonKeyBit = GetMouseButtonKeyBit(key);
			if (IsValidBit(buttonKeyBit))
			{
				return CHECK_BIT(buttonState, buttonKeyBit);
			}
			return false;
		}

		bool Win32MouseState::GetMouseWheel(const LeviathanCore::InputKey::Keys wheelKey) const
		{
			if (!IsWheelKey(wheelKey))
			{
				return false;
			}

			const unsigned char wheelBit = GetMouseButtonKeyBit(wheelKey);
			if (IsValidBit(wheelBit))
			{
				return CHECK_BIT(buttonState, wheelBit);
			}
			return false;
		}

		bool Win32MouseState::IsButtonKey(const LeviathanCore::InputKey::Keys key) const
		{
			return ((key == LeviathanCore::InputKey::Keys::LeftMouseButton) ||
				(key == LeviathanCore::InputKey::Keys::RightMouseButton) ||
				(key == LeviathanCore::InputKey::Keys::MiddleMouseButton));
		}

		bool Win32MouseState::IsWheelKey(const LeviathanCore::InputKey::Keys key) const
		{
			return ((key == LeviathanCore::InputKey::Keys::MouseWheelUpAxis) ||
				(key == LeviathanCore::InputKey::Keys::MouseWheelDownAxis));
		}

		constexpr bool Win32MouseState::IsValidBit(const uint8_t bit)
		{
			return (bit < static_cast<std::underlying_type<MouseButtonKeyBits>::type>(MouseButtonKeyBits::MaxBit));
		}

		void Win32MouseState::SetButtonKeyBit(const LeviathanCore::InputKey::Keys key)
		{
			const unsigned char buttonKeyBit = GetMouseButtonKeyBit(key);
			if (IsValidBit(buttonKeyBit))
			{
				SET_BIT(buttonState, buttonKeyBit);
			}
		}

		void Win32MouseState::ClearButtonKeyBit(const LeviathanCore::InputKey::Keys key)
		{
			const unsigned char buttonKeyBit = GetMouseButtonKeyBit(key);
			if (IsValidBit(buttonKeyBit))
			{
				CLEAR_BIT(buttonState, buttonKeyBit);
			}
		}
	}
}