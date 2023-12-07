#pragma once

#include "InputKey.h"

namespace LeviathanInput
{
	namespace PlatformInput
	{
		// Polls the current state of the keyboard and mouse to determine if key is down. Returns true if the key is 
		// currently down or false if the key is not currently down at the time of calling. Calls GetAsyncKeyState.
		BOOL IsKeyDownAsyncKeyState(const LeviathanCore::InputKey::Keys key);
	}
}