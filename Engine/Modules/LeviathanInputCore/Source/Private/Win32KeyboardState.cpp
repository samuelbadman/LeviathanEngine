#include "Win32KeyboardState.h"
#include "Bits.h"
#include "Win32Input.h"

namespace LeviathanInputCore
{
	namespace PlatformInput
	{
		// Returns the bit representing the key in the keyboard state.
		static constexpr std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type GetKeyBit(const LeviathanCore::InputKey::Keys key)
		{
			switch (key)
			{
			case LeviathanCore::InputKey::Keys::Backspace: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::BackspaceBit);
			case LeviathanCore::InputKey::Keys::Tab: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::TabBit);
			case LeviathanCore::InputKey::Keys::Enter: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::EnterBit);
			case LeviathanCore::InputKey::Keys::LeftShift: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::LeftShiftBit);
			case LeviathanCore::InputKey::Keys::RightShift: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::RightShiftBit);
			case LeviathanCore::InputKey::Keys::CapsLock: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::CapsLockBit);
			case LeviathanCore::InputKey::Keys::Escape: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::EscapeBit);
			case LeviathanCore::InputKey::Keys::SpaceBar: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::SpaceBarBit);
			case LeviathanCore::InputKey::Keys::PageUp: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::PageUpBit);
			case LeviathanCore::InputKey::Keys::PageDown: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::PageDownBit);
			case LeviathanCore::InputKey::Keys::End: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::EndBit);
			case LeviathanCore::InputKey::Keys::Home: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::HomeBit);
			case LeviathanCore::InputKey::Keys::Insert: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::InsertBit);
			case LeviathanCore::InputKey::Keys::Delete: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::DeleteBit);
			case LeviathanCore::InputKey::Keys::Left: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::LeftBit);
			case LeviathanCore::InputKey::Keys::Right: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::RightBit);
			case LeviathanCore::InputKey::Keys::Up: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::UpBit);
			case LeviathanCore::InputKey::Keys::Down: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::DownBit);
			case LeviathanCore::InputKey::Keys::Zero: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::ZeroBit);
			case LeviathanCore::InputKey::Keys::One: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::OneBit);
			case LeviathanCore::InputKey::Keys::Two: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::TwoBit);
			case LeviathanCore::InputKey::Keys::Three: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::ThreeBit);
			case LeviathanCore::InputKey::Keys::Four: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::FourBit);
			case LeviathanCore::InputKey::Keys::Five: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::FiveBit);
			case LeviathanCore::InputKey::Keys::Six: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::SixBit);
			case LeviathanCore::InputKey::Keys::Seven: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::SevenBit);
			case LeviathanCore::InputKey::Keys::Eight: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::EightBit);
			case LeviathanCore::InputKey::Keys::Nine: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::NineBit);
			case LeviathanCore::InputKey::Keys::A: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::ABit);
			case LeviathanCore::InputKey::Keys::B: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::BBit);
			case LeviathanCore::InputKey::Keys::C: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::CBit);
			case LeviathanCore::InputKey::Keys::D: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::DBit);
			case LeviathanCore::InputKey::Keys::E: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::EBit);
			case LeviathanCore::InputKey::Keys::F: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::FBit);
			case LeviathanCore::InputKey::Keys::G: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::GBit);
			case LeviathanCore::InputKey::Keys::H: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::HBit);
			case LeviathanCore::InputKey::Keys::I: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::IBit);
			case LeviathanCore::InputKey::Keys::J: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::JBit);
			case LeviathanCore::InputKey::Keys::K: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::KBit);
			case LeviathanCore::InputKey::Keys::L: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::LBit);
			case LeviathanCore::InputKey::Keys::M: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::MBit);
			case LeviathanCore::InputKey::Keys::N: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::NBit);
			case LeviathanCore::InputKey::Keys::O: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::OBit);
			case LeviathanCore::InputKey::Keys::P: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::PBit);
			case LeviathanCore::InputKey::Keys::Q: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::QBit);
			case LeviathanCore::InputKey::Keys::R: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::RBit);
			case LeviathanCore::InputKey::Keys::S: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::SBit);
			case LeviathanCore::InputKey::Keys::T: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::TBit);
			case LeviathanCore::InputKey::Keys::U: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::UBit);
			case LeviathanCore::InputKey::Keys::V: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::VBit);
			case LeviathanCore::InputKey::Keys::W: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::WBit);
			case LeviathanCore::InputKey::Keys::X: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::XBit);
			case LeviathanCore::InputKey::Keys::Y: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::YBit);
			case LeviathanCore::InputKey::Keys::Z: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::ZBit);
			case LeviathanCore::InputKey::Keys::Numpad0: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::Numpad0Bit);
			case LeviathanCore::InputKey::Keys::Numpad1: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::Numpad1Bit);
			case LeviathanCore::InputKey::Keys::Numpad2: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::Numpad2Bit);
			case LeviathanCore::InputKey::Keys::Numpad3: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::Numpad3Bit);
			case LeviathanCore::InputKey::Keys::Numpad4: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::Numpad4Bit);
			case LeviathanCore::InputKey::Keys::Numpad5: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::Numpad5Bit);
			case LeviathanCore::InputKey::Keys::Numpad6: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::Numpad6Bit);
			case LeviathanCore::InputKey::Keys::Numpad7: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::Numpad7Bit);
			case LeviathanCore::InputKey::Keys::Numpad8: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::Numpad8Bit);
			case LeviathanCore::InputKey::Keys::Numpad9: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::Numpad9Bit);
			case LeviathanCore::InputKey::Keys::F1: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::F1Bit);
			case LeviathanCore::InputKey::Keys::F2: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::F2Bit);
			case LeviathanCore::InputKey::Keys::F3: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::F3Bit);
			case LeviathanCore::InputKey::Keys::F4: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::F4Bit);
			case LeviathanCore::InputKey::Keys::F5: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::F5Bit);
			case LeviathanCore::InputKey::Keys::F6: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::F6Bit);
			case LeviathanCore::InputKey::Keys::F7: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::F7Bit);
			case LeviathanCore::InputKey::Keys::F8: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::F8Bit);
			case LeviathanCore::InputKey::Keys::F9: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::F9Bit);
			case LeviathanCore::InputKey::Keys::F10: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::F10Bit);
			case LeviathanCore::InputKey::Keys::F11: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::F11Bit);
			case LeviathanCore::InputKey::Keys::F12: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::F12Bit);
			case LeviathanCore::InputKey::Keys::LeftCtrl: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::LeftCtrlBit);
			case LeviathanCore::InputKey::Keys::RightCtrl: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::RightCtrlBit);
			case LeviathanCore::InputKey::Keys::Alt: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::AltBit);
			default: return static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::MaxBit);
			}
		}

		void Win32KeyboardState::SetKeyBit(const LeviathanCore::InputKey::Keys key)
		{
			if (LeviathanCore::InputKey::IsKeyboardKey(key))
			{
				const uint8_t bit = GetKeyBit(key);
				if (IsValidBit(bit))
				{
					if (IsState1(bit))
					{
						LEVIATHAN_SET_BIT(State1, bit);
					}
					else
					{
						LEVIATHAN_SET_BIT(State2, bit);
					}
				}
			}
		}

		void Win32KeyboardState::ClearKeyBit(const LeviathanCore::InputKey::Keys key)
		{
			if (LeviathanCore::InputKey::IsKeyboardKey(key))
			{
				const uint8_t bit = GetKeyBit(key);
				if (IsValidBit(bit))
				{
					if (IsState1(bit))
					{
						LEVIATHAN_CLEAR_BIT(State1, bit);
					}
					else
					{
						LEVIATHAN_CLEAR_BIT(State2, bit);
					}
				}
			}
		}

		void Win32KeyboardState::UpdateKeyState(const LeviathanCore::InputKey::Keys key)
		{
			if (PlatformInput::IsKeyDownAsyncKeyState(key))
			{
				SetKeyBit(key);
			}
			else
			{
				ClearKeyBit(key);
			}
		}

		bool Win32KeyboardState::IsKeyPressed(const LeviathanCore::InputKey::Keys key) const
		{
			if (!LeviathanCore::InputKey::IsKeyboardKey(key))
			{
				return false;
			}

			const unsigned char bit = GetKeyBit(key);

			if (IsValidBit(bit))
			{
				if (IsState1(bit))
				{
					return LEVIATHAN_CHECK_BIT(State1, bit);
				}
				else
				{
					return LEVIATHAN_CHECK_BIT(State2, bit);
				}
			}

			return false;
		}

		bool Win32KeyboardState::GetKeyState(const LeviathanCore::InputKey::Keys key)
		{
			const unsigned char bit = GetKeyBit(key);

			if (IsValidBit(bit))
			{
				if (IsState1(bit))
				{
					return LEVIATHAN_CHECK_BIT(State1, bit);
				}
				else
				{
					return LEVIATHAN_CHECK_BIT(State2, bit);
				}
			}

			return false;
		}

		void Win32KeyboardState::SetKeyState(const LeviathanCore::InputKey::Keys key, bool newState)
		{
			if (newState)
			{
				SetKeyBit(key);
			}
			else
			{
				ClearKeyBit(key);
			}
		}

		constexpr bool Win32KeyboardState::IsState1(const uint8_t bit)
		{
			return (bit <= 63);
		}

		constexpr bool Win32KeyboardState::IsValidBit(const uint8_t bit)
		{
			return (bit < static_cast<std::underlying_type<Win32KeyboardState::KeyboardKeyBits>::type>(Win32KeyboardState::KeyboardKeyBits::MaxBit));
		}
	}
}