#pragma once

#include "LeviathanCore.h"

namespace LeviathanInput
{
	namespace PlatformInput
	{
		using InputCallbackType = void(*)(LeviathanCore::InputKey /* key */, bool /* isRepeatKey */, float /* data */);
		using GameControllerInputCallbackType = void(*)(LeviathanCore::InputKey /* key */, bool /* isRepeatKey */, float /* data */, unsigned int /* gameControllerId */);
		using GameControllerConnectedCallbackType = void(*)(unsigned int /* gameControllerId */);
		using GameControllerDisconnectedCallbackType = void(*)(unsigned int /* gameControllerId */);

		bool Initialize();
		bool Shutdown();
		void DispatchCallbackForKey(const LeviathanCore::InputKey::Keys key);
		void DispatchCallbackForGameControllerKey(const LeviathanCore::InputKey::Keys key, const unsigned int gameControllerId);
		LeviathanCore::Callback<InputCallbackType>& GetInputCallback();
		LeviathanCore::Callback<GameControllerInputCallbackType>& GetGameControllerInputCallback();
		LeviathanCore::Callback<GameControllerConnectedCallbackType>& GetGameControllerConnectedCallback();
		LeviathanCore::Callback<GameControllerDisconnectedCallbackType>& GetGameControllerDisconnectedCallback();
	}
}