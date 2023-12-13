#include "XInputGamepad.h"
#include "Bits.h"

namespace LeviathanInputCore
{
	namespace PlatformInput
	{
		namespace XInputGamepad
		{
			static constexpr unsigned char MaxGamepadCount = XUSER_MAX_COUNT;
			static constexpr unsigned short MaxStickMagnitude = 32767;
			static constexpr unsigned char MaxTriggerMagnitude = 255;
			static constexpr float LeftThumbsticDeadzonePercentageX = 0.05f;
			static constexpr float LeftThumbsticDeadzonePercentageY = 0.05f;
			static constexpr float RightThumbsticDeadzonePercentageX = 0.07f;
			static constexpr float RightThumbsticDeadzonePercentageY = 0.11f;

			// Callback objects.
			static LeviathanCore::Callback<XInputGamepadInputCallbackType> XInputGamepadInputCallback = {};
			static LeviathanCore::Callback<XInputGamepadConnectedCallbackType> XInputGamepadConnectedInputCallback = {};
			static LeviathanCore::Callback<XInputGamepadDisconnectedCallbackType> XInputGamepadDisconnectedInputCallback = {};

			// Store flags as bits for each supported gamepad. Only 4 gamepads are supported, requiring 4 bits.
			static unsigned char ConnectedGamepadBits = 0;

			// XInput gamepad states the previous time UpdatePreviousConnectedGamepadStates was called.
			static std::array<XINPUT_STATE, MaxGamepadCount> PreviousStates =
			{
				XINPUT_STATE{},
				XINPUT_STATE{},
				XINPUT_STATE{},
				XINPUT_STATE{}
			};

			// XInput gamepad states the previous time RefreshConnectedGamepadStates was called.
			static std::array<XINPUT_STATE, MaxGamepadCount> CurrentStates =
			{
				XINPUT_STATE{},
				XINPUT_STATE{},
				XINPUT_STATE{},
				XINPUT_STATE{}
			};

			// Returns true if a gamepad is connected at userIndex otherwise, returns false if a gamepad is not connected at userIndex.
			static bool IsGamepadConnected(const DWORD userIndex)
			{
				if (userIndex >= MaxGamepadCount)
				{
					return false;
				}
				return (CHECK_BIT(ConnectedGamepadBits, userIndex) == 1);
			}

			// Sends messages for the current state of a button on the game controller connected at userIndex. The button messages are generated for is specified by key.
			// Fails silently if buttonKey is not a valid gamepad button key.
			static void CheckButton(const LeviathanCore::InputKey::Keys buttonKey, const WORD previousButtonsState, const WORD currentButtonsState, const DWORD userIndex)
			{
				if ((buttonKey != LeviathanCore::InputKey::Keys::GamepadFaceButtonBottom) &&
					(buttonKey != LeviathanCore::InputKey::Keys::GamepadFaceButtonLeft) &&
					(buttonKey != LeviathanCore::InputKey::Keys::GamepadFaceButtonTop) &&
					(buttonKey != LeviathanCore::InputKey::Keys::GamepadFaceButtonRight) &&
					(buttonKey != LeviathanCore::InputKey::Keys::GamepadDPadDown) &&
					(buttonKey != LeviathanCore::InputKey::Keys::GamepadDPadLeft) &&
					(buttonKey != LeviathanCore::InputKey::Keys::GamepadDPadUp) &&
					(buttonKey != LeviathanCore::InputKey::Keys::GamepadDPadRight) &&
					(buttonKey != LeviathanCore::InputKey::Keys::GamepadSpecialLeft) &&
					(buttonKey != LeviathanCore::InputKey::Keys::GamepadSpecialRight) &&
					(buttonKey != LeviathanCore::InputKey::Keys::GamepadLeftShoulder) &&
					(buttonKey != LeviathanCore::InputKey::Keys::GamepadRightShoulder) &&
					(buttonKey != LeviathanCore::InputKey::Keys::GamepadLeftThumbstickButton) &&
					(buttonKey != LeviathanCore::InputKey::Keys::GamepadRightThumbstickButton))
				{
					return;
				}

				const int previousButtonState = previousButtonsState & static_cast<WORD>(buttonKey);
				const int currentButtonState = currentButtonsState & static_cast<WORD>(buttonKey);
				if (currentButtonState != 0)
				{
					// Pressed.
					GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(buttonKey), (previousButtonState == currentButtonState), 1.0f, static_cast<unsigned int>(userIndex));
				}
				else
				{
					// Released.
					if (previousButtonState != currentButtonState)
					{
						GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(buttonKey), false, 0.0f, static_cast<unsigned int>(userIndex));
					}
				}
			}

			// Applies a deadzone with the given percentage to the out value.
			static void ApplyDeadzone(const float deadzonePercentage, float& outX)
			{
				const float normalized = std::max(-1.0f, outX / static_cast<float>(MaxStickMagnitude));
				const float absNormalized = std::abs(normalized);
				outX = (absNormalized < deadzonePercentage ? 0.0f : (absNormalized - deadzonePercentage) * (normalized / absNormalized));
				if (deadzonePercentage > 0.0f)
				{
					outX /= 1.0f - deadzonePercentage;
				}
			}

			// Sends messages for the current state of the analog inputs of the thumbstick on the gamepad connected at userIndex. Fails silently if 
			// analogThumbstickKey is not a valid analog thumbstick key.
			static void CheckThumbstickAnalogInputs(const short current, const float deadzonePercentage, const LeviathanCore::InputKey::Keys analogThumbstickKey, const DWORD userIndex)
			{
				// Analog stick input.
				if ((analogThumbstickKey != LeviathanCore::InputKey::Keys::GamepadLeftThumbstickXAxis) &&
					(analogThumbstickKey != LeviathanCore::InputKey::Keys::GamepadLeftThumbstickYAxis) &&
					(analogThumbstickKey != LeviathanCore::InputKey::Keys::GamepadRightThumbstickXAxis) &&
					(analogThumbstickKey != LeviathanCore::InputKey::Keys::GamepadRightThumbstickYAxis))
				{
					return;
				}

				float fCurrent = static_cast<float>(current);
				ApplyDeadzone(deadzonePercentage, fCurrent);
				GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(analogThumbstickKey), false, fCurrent, static_cast<unsigned int>(userIndex));
			}

			// Sends messages for the current state of the digital inputs of the thumbstick on the gamepad connected at userIndex. Fails silently if 
			// digitalThumbstickKey is not a valid digital thumbstick key.
			static void CheckThumbstickDigitalInputs(const short previousX, const short previousY, const short currentX, const short currentY,
				const LeviathanCore::InputKey::Keys digitalThumbstickKey, const DWORD userIndex)
			{
				// Digital stick input
				if ((digitalThumbstickKey != LeviathanCore::InputKey::Keys::GamepadLeftThumbstickUp) &&
					(digitalThumbstickKey != LeviathanCore::InputKey::Keys::GamepadLeftThumbstickDown) &&
					(digitalThumbstickKey != LeviathanCore::InputKey::Keys::GamepadLeftThumbstickLeft) &&
					(digitalThumbstickKey != LeviathanCore::InputKey::Keys::GamepadLeftThumbstickRight) &&
					(digitalThumbstickKey != LeviathanCore::InputKey::Keys::GamepadRightThumbstickUp) &&
					(digitalThumbstickKey != LeviathanCore::InputKey::Keys::GamepadRightThumbstickDown) &&
					(digitalThumbstickKey != LeviathanCore::InputKey::Keys::GamepadRightThumbstickLeft) &&
					(digitalThumbstickKey != LeviathanCore::InputKey::Keys::GamepadRightThumbstickRight))
				{
					return;
				}

				// X
				const short normalizedPreviousX = previousX / MaxStickMagnitude;
				const short normalizedCurrentX = currentX / MaxStickMagnitude;

				if (normalizedCurrentX > 0)
				{
					// Pushed right.
					// Filter for desired key before sending message.
					if ((digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadLeftThumbstickRight) || 
						(digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadRightThumbstickRight))
					{
						GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(digitalThumbstickKey), (normalizedPreviousX == normalizedCurrentX), 1.0f, static_cast<unsigned int>(userIndex));
					}
				}
				else if (normalizedCurrentX < 0)
				{
					// Pushed left.
					// Filter for desired key before sending message.
					if ((digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadLeftThumbstickLeft) ||
						(digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadRightThumbstickLeft))
					{
						GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(digitalThumbstickKey), (normalizedPreviousX == normalizedCurrentX), 1.0f, static_cast<unsigned int>(userIndex));
					}
				}
				else
				{
					if (normalizedPreviousX > 0)
					{
						// Was pushed right.
						// Filter for desired key before sending message.
						if ((digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadLeftThumbstickRight) || 
							(digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadRightThumbstickRight))
						{
							GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(digitalThumbstickKey), false, 0.0f, static_cast<unsigned int>(userIndex));
						}
					}
					else if (normalizedPreviousX < 0)
					{
						// Was pushed left.
						// Filter for desired key before sending message.
						if ((digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadLeftThumbstickLeft) || 
							(digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadRightThumbstickLeft))
						{
							GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(digitalThumbstickKey), false, 0.0f, static_cast<unsigned int>(userIndex));
						}
					}
				}

				// Y
				const int16_t normalizedPreviousY = previousY / MaxStickMagnitude;
				const int16_t normalizedCurrentY = currentY / MaxStickMagnitude;

				if (normalizedCurrentY > 0)
				{
					// Pushed up.
					if ((digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadLeftThumbstickUp) || 
						(digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadRightThumbstickUp))
					{
						GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(digitalThumbstickKey), (normalizedPreviousY == normalizedCurrentY), 1.0f, static_cast<unsigned int>(userIndex));
					}
				}
				else if (normalizedCurrentY < 0)
				{
					// Pushed down.
					if ((digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadLeftThumbstickDown) || 
						(digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadRightThumbstickDown))
					{
						GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(digitalThumbstickKey), (normalizedPreviousY == normalizedCurrentY), 1.0f, static_cast<unsigned int>(userIndex));
					}
				}
				else
				{
					if (normalizedPreviousY > 0)
					{
						// Was pushed up.
						if ((digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadLeftThumbstickUp) || 
							(digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadRightThumbstickUp))
						{
							GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(digitalThumbstickKey), false, 0.0f, static_cast<unsigned int>(userIndex));
						}
					}
					else if (normalizedPreviousY < 0)
					{
						// Was pushed down.
						if ((digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadLeftThumbstickDown) || 
							(digitalThumbstickKey == LeviathanCore::InputKey::Keys::GamepadRightThumbstickDown))
						{
							GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(digitalThumbstickKey), false, 0.0f, static_cast<unsigned int>(userIndex));
						}
					}
				}
			}

			// Sends messages for the current state of the trigger's axis input for the gamepad connected at userIndex. Fails silently if 
			// analogTriggerKey is not a valid analog trigger key.
			static void CheckTriggerAnalogInput(const BYTE currentTrigger, const LeviathanCore::InputKey::Keys analogTriggerKey, const DWORD userIndex)
			{
				// Analog trigger input
				if ((analogTriggerKey != LeviathanCore::InputKey::Keys::GamepadLeftTriggerAxis) &&
					(analogTriggerKey != LeviathanCore::InputKey::Keys::GamepadRightTriggerAxis))
				{
					return;
				}

				GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(analogTriggerKey),
					false,
					static_cast<float>(currentTrigger) / static_cast<float>(MaxTriggerMagnitude),
					static_cast<unsigned int>(userIndex));
			}

			// Sends messages for the current state of the trigger's digital input for the gamepad connected at userIndex. Silently fails if 
			// digitalTriggerKey is not a valid trigger digital key.
			static void CheckTriggerDigitalInput(const BYTE previousTrigger, const BYTE currentTrigger, const LeviathanCore::InputKey::Keys digitalTriggerKey, const DWORD userIndex)
			{
				// Digital trigger input
				if ((digitalTriggerKey != LeviathanCore::InputKey::Keys::GamepadLeftTrigger) &&
					(digitalTriggerKey != LeviathanCore::InputKey::Keys::GamepadRightTrigger))
				{
					return;
				}

				const uint8_t normalizedPreviousTrigger = previousTrigger / MaxTriggerMagnitude;
				const uint8_t normalizedCurrentTrigger = currentTrigger / MaxTriggerMagnitude;

				if (normalizedPreviousTrigger != normalizedCurrentTrigger)
				{
					// Pressed/Released.
					GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(digitalTriggerKey),
						false,
						static_cast<float>(normalizedCurrentTrigger),
						static_cast<unsigned int>(userIndex));
				}

				if (normalizedCurrentTrigger == 1)
				{
					// Repeated press.
					GetXInputGamepadInputCallback().Call(LeviathanCore::InputKey(digitalTriggerKey),
						true,
						static_cast<float>(normalizedCurrentTrigger),
						static_cast<uint32_t>(userIndex));
				}
			}

			void Initialize()
			{
				ConnectedGamepadBits = 0;
				GetXInputGamepadInputCallback().Clear();
				GetXInputGamepadConnectedCallback().Clear();
				GetXInputGamepadDisconnectedCallback().Clear();
				std::fill(PreviousStates.begin(), PreviousStates.end(), XINPUT_STATE{});
				std::fill(CurrentStates.begin(), CurrentStates.end(), XINPUT_STATE{});
			}

			void RefreshConnectedGamepadFlags()
			{
				XINPUT_STATE state = {};
				for (DWORD i = 0; i < MaxGamepadCount; ++i)
				{
					switch (XInputGetState(i, &state))
					{
					case ERROR_SUCCESS:
						// Gamepad is connected. Set corresponding bit in flag integer if corresponding flag bit is cleared.
						if (!IsGamepadConnected(i))
						{
							SET_BIT(ConnectedGamepadBits, i);
							GetXInputGamepadConnectedCallback().Call(static_cast<unsigned int>(i));
						}
						break;

					case ERROR_DEVICE_NOT_CONNECTED:
						// Gamepad is not connected. Clear corresponding bit in flag integer if corresponding flag bit is set.
						if (IsGamepadConnected(i))
						{
							CLEAR_BIT(ConnectedGamepadBits, i);
							GetXInputGamepadDisconnectedCallback().Call(static_cast<unsigned int>(i));
						}
						break;

					default:
						// XInputGetState failed.
						break;
					}
				}
			}

			void LogConnectedGamepads()
			{
				//for (DWORD i = 0; i < MaxGamepadCount; ++i)
				//{
				//	if (IsGamepadConnected(i))
				//	{
				//		printf("Game controller %d is connected.", i);
				//	}
				//	else
				//	{
				//		printf("Game controller %d is not connected.", i);
				//	}
				//}
			}

			void RefreshConnectedGamepadStates()
			{
				for (DWORD i = 0; i < MaxGamepadCount; ++i)
				{
					if (IsGamepadConnected(i))
					{
						XInputGetState(i, &CurrentStates[i]);
					}
				}
			}

			void UpdatePreviousConnectedGamepadStates()
			{
				for (DWORD i = 0; i < MaxGamepadCount; ++i)
				{
					if (IsGamepadConnected(i))
					{
						PreviousStates[i] = CurrentStates[i];
					}
				}
			}

			void DispatchMessagesForGamepadKey(const LeviathanCore::InputKey::Keys key, const unsigned int gameControllerId)
			{
				if (!LeviathanCore::InputKey::IsGamepadKey(key))
				{
					return;
				}

				if (!IsGamepadConnected(static_cast<DWORD>(gameControllerId)))
				{
					return;
				}

				switch (key)
				{
				case LeviathanCore::InputKey::Keys::GamepadFaceButtonBottom:
				case LeviathanCore::InputKey::Keys::GamepadFaceButtonLeft:
				case LeviathanCore::InputKey::Keys::GamepadFaceButtonTop:
				case LeviathanCore::InputKey::Keys::GamepadFaceButtonRight:
				case LeviathanCore::InputKey::Keys::GamepadDPadDown:
				case LeviathanCore::InputKey::Keys::GamepadDPadLeft:
				case LeviathanCore::InputKey::Keys::GamepadDPadUp:
				case LeviathanCore::InputKey::Keys::GamepadDPadRight:
				case LeviathanCore::InputKey::Keys::GamepadSpecialLeft:
				case LeviathanCore::InputKey::Keys::GamepadSpecialRight:
				case LeviathanCore::InputKey::Keys::GamepadLeftShoulder:
				case LeviathanCore::InputKey::Keys::GamepadRightShoulder:
				case LeviathanCore::InputKey::Keys::GamepadLeftThumbstickButton:
				case LeviathanCore::InputKey::Keys::GamepadRightThumbstickButton:
				{
					CheckButton(key,
						PreviousStates[static_cast<size_t>(gameControllerId)].Gamepad.wButtons,
						CurrentStates[static_cast<size_t>(gameControllerId)].Gamepad.wButtons,
						static_cast<DWORD>(gameControllerId));

					break;
				}

				case LeviathanCore::InputKey::Keys::GamepadLeftThumbstickXAxis:
				{
					CheckThumbstickAnalogInputs(CurrentStates[static_cast<size_t>(gameControllerId)].Gamepad.sThumbLX,
						LeftThumbsticDeadzonePercentageX,
						key,
						static_cast<DWORD>(gameControllerId));

					break;
				}

				case LeviathanCore::InputKey::Keys::GamepadLeftThumbstickYAxis:
				{
					CheckThumbstickAnalogInputs(CurrentStates[static_cast<size_t>(gameControllerId)].Gamepad.sThumbLY,
						LeftThumbsticDeadzonePercentageY,
						key,
						static_cast<DWORD>(gameControllerId));

					break;
				}

				case LeviathanCore::InputKey::Keys::GamepadRightThumbstickXAxis:
				{
					CheckThumbstickAnalogInputs(CurrentStates[static_cast<size_t>(gameControllerId)].Gamepad.sThumbRX,
						RightThumbsticDeadzonePercentageX,
						key,
						static_cast<DWORD>(gameControllerId));

					break;
				}

				case LeviathanCore::InputKey::Keys::GamepadRightThumbstickYAxis:
				{
					CheckThumbstickAnalogInputs(CurrentStates[static_cast<size_t>(gameControllerId)].Gamepad.sThumbRY,
						RightThumbsticDeadzonePercentageY,
						key,
						static_cast<DWORD>(gameControllerId));

					break;
				}

				case LeviathanCore::InputKey::Keys::GamepadLeftThumbstickLeft:
				case LeviathanCore::InputKey::Keys::GamepadLeftThumbstickRight:
				case LeviathanCore::InputKey::Keys::GamepadLeftThumbstickUp:
				case LeviathanCore::InputKey::Keys::GamepadLeftThumbstickDown:
				{
					CheckThumbstickDigitalInputs(PreviousStates[static_cast<size_t>(gameControllerId)].Gamepad.sThumbLX,
						PreviousStates[static_cast<size_t>(gameControllerId)].Gamepad.sThumbLY,
						CurrentStates[static_cast<size_t>(gameControllerId)].Gamepad.sThumbLX,
						CurrentStates[static_cast<size_t>(gameControllerId)].Gamepad.sThumbLY,
						key,
						static_cast<DWORD>(gameControllerId));

					break;
				}

				case LeviathanCore::InputKey::Keys::GamepadRightThumbstickLeft:
				case LeviathanCore::InputKey::Keys::GamepadRightThumbstickRight:
				case LeviathanCore::InputKey::Keys::GamepadRightThumbstickUp:
				case LeviathanCore::InputKey::Keys::GamepadRightThumbstickDown:
				{
					CheckThumbstickDigitalInputs(PreviousStates[static_cast<size_t>(gameControllerId)].Gamepad.sThumbRX,
						PreviousStates[static_cast<size_t>(gameControllerId)].Gamepad.sThumbRY,
						CurrentStates[static_cast<size_t>(gameControllerId)].Gamepad.sThumbRX,
						CurrentStates[static_cast<size_t>(gameControllerId)].Gamepad.sThumbRY,
						key,
						static_cast<DWORD>(gameControllerId));

					break;
				}

				case LeviathanCore::InputKey::Keys::GamepadLeftTrigger:
				{
					CheckTriggerDigitalInput(PreviousStates[static_cast<size_t>(gameControllerId)].Gamepad.bLeftTrigger,
						CurrentStates[static_cast<size_t>(gameControllerId)].Gamepad.bLeftTrigger,
						key,
						static_cast<DWORD>(gameControllerId));

					break;
				}

				case LeviathanCore::InputKey::Keys::GamepadLeftTriggerAxis:
				{
					CheckTriggerAnalogInput(CurrentStates[static_cast<size_t>(gameControllerId)].Gamepad.bLeftTrigger,
						key,
						static_cast<DWORD>(gameControllerId));

					break;
				}

				case LeviathanCore::InputKey::Keys::GamepadRightTrigger:
				{
					CheckTriggerDigitalInput(PreviousStates[static_cast<size_t>(gameControllerId)].Gamepad.bRightTrigger,
						CurrentStates[static_cast<size_t>(gameControllerId)].Gamepad.bRightTrigger,
						key,
						static_cast<DWORD>(gameControllerId));

					break;
				}

				case LeviathanCore::InputKey::Keys::GamepadRightTriggerAxis:
				{
					CheckTriggerAnalogInput(CurrentStates[static_cast<size_t>(gameControllerId)].Gamepad.bRightTrigger,
						key,
						static_cast<DWORD>(gameControllerId));

					break;
				}

				default:
				{
					break;
				}
				}
			}

			bool SetVibration(const unsigned short leftMotorSpeed, const unsigned short rightMotorSpeed, const unsigned int gameControllerId)
			{
				if (IsGamepadConnected(static_cast<DWORD>(gameControllerId)))
				{
					XINPUT_VIBRATION vibration = {};
					vibration.wLeftMotorSpeed = leftMotorSpeed;
					vibration.wRightMotorSpeed = rightMotorSpeed;
					return XInputSetState(static_cast<DWORD>(gameControllerId), &vibration) == ERROR_SUCCESS;
				}

				return false;
			}

			LeviathanCore::Callback<XInputGamepadInputCallbackType>& GetXInputGamepadInputCallback()
			{
				return XInputGamepadInputCallback;
			}

			LeviathanCore::Callback<XInputGamepadConnectedCallbackType>& GetXInputGamepadConnectedCallback()
			{
				return XInputGamepadConnectedInputCallback;
			}

			LeviathanCore::Callback<XInputGamepadDisconnectedCallbackType>& GetXInputGamepadDisconnectedCallback()
			{
				return XInputGamepadDisconnectedInputCallback;
			}

		}
	}
}