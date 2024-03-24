#pragma once

#include "InputKey.h"

namespace LeviathanInputCore
{
	namespace PlatformInput
	{
		// Class containing 20 bytes/80 bits used to store 78 bits that represent the down/up state of each keyboard key.
		class Win32KeyboardState
		{
		private:
			unsigned long long State1 = 0; // 64 bits. Stores down/up state for 64 keys. Bit 0-64 is used.
			unsigned short State2 = 0; // 16 bits. Stores down/up state for 14 keys. Bit 0-13 is used.

		public:
			enum class KeyboardKeyBits : unsigned char
			{
				BackspaceBit = 0,
				TabBit = 1,
				EnterBit = 2,
				LeftShiftBit = 3,
				RightShiftBit = 4,
				CapsLockBit = 5,
				EscapeBit = 6,
				SpaceBarBit = 7,
				PageUpBit = 8,
				PageDownBit = 9,
				EndBit = 10,
				HomeBit = 11,
				InsertBit = 12,
				DeleteBit = 13,
				LeftBit = 14,
				RightBit = 15,
				UpBit = 16,
				DownBit = 17,
				ZeroBit = 18,
				OneBit = 19,
				TwoBit = 20,
				ThreeBit = 21,
				FourBit = 22,
				FiveBit = 23,
				SixBit = 24,
				SevenBit = 25,
				EightBit = 26,
				NineBit = 27,
				ABit = 28,
				BBit = 29,
				CBit = 30,
				DBit = 31,
				EBit = 32,
				FBit = 33,
				GBit = 34,
				HBit = 35,
				IBit = 36,
				JBit = 37,
				KBit = 38,
				LBit = 39,
				MBit = 40,
				NBit = 41,
				OBit = 42,
				PBit = 43,
				QBit = 44,
				RBit = 45,
				SBit = 46,
				TBit = 47,
				UBit = 48,
				VBit = 49,
				WBit = 50,
				XBit = 51,
				YBit = 52,
				ZBit = 53,
				Numpad0Bit = 54,
				Numpad1Bit = 55,
				Numpad2Bit = 56,
				Numpad3Bit = 57,
				Numpad4Bit = 58,
				Numpad5Bit = 59,
				Numpad6Bit = 60,
				Numpad7Bit = 61,
				Numpad8Bit = 62,
				Numpad9Bit = 63,
				F1Bit = 64,
				F2Bit = 65,
				F3Bit = 66,
				F4Bit = 67,
				F5Bit = 68,
				F6Bit = 69,
				F7Bit = 70,
				F8Bit = 71,
				F9Bit = 72,
				F10Bit = 73,
				F11Bit = 74,
				F12Bit = 75,
				LeftCtrlBit = 76,
				RightCtrlBit = 77,
				AltBit = 78,
				MaxBit = 79
			};

			// Updates the state of the bit representing key storing the up/down state of the key since the last call to UpdateKeyState.
			void UpdateKeyState(const LeviathanCore::InputKey::Keys key);

			// Returns true if the bit corresponding to key is set otherwise, returns false if 
			// the bit corresponding to key is cleared or key is not a keyboard key.
			bool IsKeyPressed(const LeviathanCore::InputKey::Keys key) const;

			// Retreives the set state of the corresponding bit for the key. Returns true if the bit is set otherwise, returns false if the bit is not set or the key is invalid.
			bool GetKeyState(const LeviathanCore::InputKey::Keys key);

			// Manually sets the set state of the corresponding bit for the key. If newState is true, the bit will be set otherwise, the bit will be cleared.
			void SetKeyState(const LeviathanCore::InputKey::Keys key, bool newState);

		private:
			// Returns true if bit bit is stored in State1 otherwise, returns false if the bit is stored in state2.
			static bool constexpr IsState1(const unsigned char bit);

			// Returns true if bit is less than MaxBit otherwise, returns false.
			static bool constexpr IsValidBit(const unsigned char bit);

			// Sets the bit corresponding to key. Silently fails if key does not produce a valid bit.
			void SetKeyBit(const LeviathanCore::InputKey::Keys key);

			// Clears the bit corresponding to key. Silently fails if key does not produce a valid bit.
			void ClearKeyBit(const LeviathanCore::InputKey::Keys key);
		};
	}
}