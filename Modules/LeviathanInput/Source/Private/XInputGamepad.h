#pragma once

#include "InputKey.h"
#include "Callback.h"

namespace LeviathanInput
{
	namespace PlatformInput
	{
		namespace XInputGamepad
		{
			using XInputGamepadInputCallbackType = void(*)(LeviathanCore::InputKey /* key */, bool /* isRepeatKey */, float /* data */, unsigned int /* gameControllerId */);
			using XInputGamepadConnectedCallbackType = void(*)(unsigned int /* gameControllerId */);
			using XInputGamepadDisconnectedCallbackType = void(*)(unsigned int /* gameControllerId */);

			// Initializes the xinput implementation.
			void Initialize();

			// Refreshes flags for identifying connected game controller ids.
			void RefreshConnectedGameControllerFlags();

			// Logs a message to the engine's logging system stating the connection state of each supported game controller.
			void LogConnectedGameControllers();

			// Refreshes current states for connected game controllers.
			void RefreshConnectedGameControllerStates();

			// Copies current states for connected game controllers to previous game controller states.
			void UpdatePreviousConnectedGameControllerStates();

			//// Sends messages for each connected game controller.
			//void PumpConnectedGameControllerMessages();

			// Sends messages for key for game controller with gameControllerId.
			void DispatchMessagesForGameControllerKey(const LeviathanCore::InputKey::Keys key, const unsigned int gameControllerId);

			// Sets motor speeds for the game controller connected at gameControllerId. Returns true if the function succeeds otherwise,
			// returns false.
			bool SetVibration(const unsigned short leftMotorSpeed, const unsigned short rightMotorSpeed, const unsigned int gameControllerId);

			// Retreives the game controller input callback object.
			LeviathanCore::Callback<XInputGamepadInputCallbackType>& GetXInputGamepadInputCallback();

			// Retreives the game controller connected callback object.
			LeviathanCore::Callback<XInputGamepadConnectedCallbackType>& GetXInputGamepadConnectedCallback();

			// Retreives the game controller disconnected callback object.
			LeviathanCore::Callback<XInputGamepadDisconnectedCallbackType>& GetXInputGamepadDisconnectedCallback();
		}
	}
}