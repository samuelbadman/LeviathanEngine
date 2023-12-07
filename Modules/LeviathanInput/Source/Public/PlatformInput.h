#pragma once

#include "LeviathanCore.h"

namespace LeviathanInput
{
	namespace PlatformInput
	{
		using InputCallbackType = void(*)(LeviathanCore::InputKey /* key */, bool /* isRepeatKey */, float /* data */);

		bool Initialize();
		bool Shutdown();
		void DispatchCallbackForKey(const LeviathanCore::InputKey::Keys key);
		LeviathanCore::Callback<InputCallbackType>& GetInputCallback();
	}
}