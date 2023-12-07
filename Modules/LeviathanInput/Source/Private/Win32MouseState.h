#pragma once

#include "InputKey.h"

namespace LeviathanInput
{
	namespace PlatformInput
	{
		// Class containing mouse state data and helper functions for working with that data.
		class Win32MouseState
		{
		private:
			// 1 byte/8 bits to store mouse button states.
			unsigned char buttonState = 0;

			// Mouse position xy coordinates across all connected displays relative to primary display.
			std::array<long long, 2> position = { 0, 0 };

		public:
			enum class MouseButtonKeyBits : unsigned char
			{
				LeftMouseButtonBit = 0,
				RightMouseButtonBit = 1,
				MiddleMouseButtonBit = 2,
				MouseWheelUpAxisBit = 3,
				MouseWheelDownAxisBit = 4,
				MaxBit = 5
			};

			// Updates the mouse wheel bit states. If setBit is true the bit corresponding to the mouse wheel key is set otherwise, the 
			// bit corresponding to the mouse wheel key is cleared. Silently fails if wheelKey is not a mouse wheel axis key or an invalid bit
			// is produced.
			void UpdateMouseWheel(const LeviathanCore::InputKey::Keys wheelKey, const bool setBit);

			// Updates the mouse position coordinates.
			void UpdatePosition(const long long x, const long long y);

			// Updates the state of the button bit representing key storing the up/down state of the key since the last call to UpdateButtonState.
			// Silently fails if key is not a mouse button (left, right, middle) key or does not produce a valid bit.
			void UpdateButtonState(const LeviathanCore::InputKey::Keys key);

			// Returns true if the bit corresponding to key is set. Otherwise, returns false if 
			// the bit corresponding to key is cleared or key is not a mouse button key.
			bool IsButtonPressed(const LeviathanCore::InputKey::Keys key) const;

			// Returns true if the bit corresponding to the mouse wheel key is set otherwise, returns false if the bit correspoonding to the mouse
			// wheel key is cleared, the mouse wheel key is not a mouse wheel key or the mouse wheel key produces an invalid bit.
			bool GetMouseWheel(const LeviathanCore::InputKey::Keys wheelKey) const;

			// Returns the mouse state's last x position coordinate.
			int64_t GetMouseXPosition() const { return position[0]; }

			// Returns the mouse state's last y position coordinate.
			int64_t GetMouseYPosition() const { return position[1]; }

		private:
			// Returns true if the key is a mouse button key otherwise, returns false if key is not a mouse button key.
			bool IsButtonKey(const LeviathanCore::InputKey::Keys key) const;

			// Returns true if the key is a mouse wheel axis key otherwise, returns false if key is not a mouse wheel axis key.
			bool IsWheelKey(const LeviathanCore::InputKey::Keys key) const;

			// Returns true if bit is less than maxBit otherwise, returns false.
			static bool constexpr IsValidBit(const unsigned char bit);

			// Sets the button bit corresponding to key. Silently fails if key does not produce a valid bit.
			void SetButtonKeyBit(const LeviathanCore::InputKey::Keys key);

			// Clears the button bit corresponding to key. Silently fails if key does not produce a valid bit.
			void ClearButtonKeyBit(const LeviathanCore::InputKey::Keys key);
		};
	}
}