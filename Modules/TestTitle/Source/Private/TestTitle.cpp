#include "TestTitle.h"
#include "LeviathanCore.h"
#include "LeviathanInput.h"

namespace TestTitle
{
	static bool Shutdown()
	{
		if (!LeviathanInput::Shutdown())
		{
			return false;
		}

		return true;
	}

	static void OnPreMainLoop()
	{

	}

	static void OnPostMainLoop()
	{

	}

	static void OnPreTick()
	{

	}

	static void OnTick([[maybe_unused]] float DeltaSeconds)
	{
		LeviathanInput::PlatformInput::DispatchCallbackForKey(LeviathanCore::InputKey::Keys::A);
	}

	static void OnPostTick()
	{
	}

	static void OnCleanup()
	{
		Shutdown();
	}

	static void OnInput(LeviathanCore::InputKey key, bool isRepeatKey, float data)
	{
		if (key.GetKey() == LeviathanCore::InputKey::Keys::A)
		{
			std::cout << "A key input. isRepeatKey " << ((isRepeatKey) ? "true" : "false") << " data " << data << '\n';
		}
	}

	static void OnGameControllerInput(LeviathanCore::InputKey key, bool isRepeatKey, float data, unsigned int gameControllerId)
	{

	}

	static void OnGameControllerConnected(unsigned int gameControllerId)
	{

	}

	static void OnGameControllerDisconnected(unsigned int gameControllerId)
	{

	}

	bool Initialize()
	{
		// Initialize engine modules for title.
		LeviathanInput::Initialize();

		// Register callbacks.
		LeviathanCore::Core::GetPreMainLoopCallback().Register(OnPreMainLoop);
		LeviathanCore::Core::GetPostMainLoopCallback().Register(OnPostMainLoop);
		LeviathanCore::Core::GetPreTickCallback().Register(OnPreTick);
		LeviathanCore::Core::GetTickCallback().Register(OnTick);
		LeviathanCore::Core::GetPostTickCallback().Register(OnPostTick);
		LeviathanCore::Core::GetCleanupCallback().Register(OnCleanup);

		LeviathanInput::PlatformInput::GetInputCallback().Register(OnInput);
		LeviathanInput::PlatformInput::GetGameControllerInputCallback().Register(OnGameControllerInput);
		LeviathanInput::PlatformInput::GetGameControllerConnectedCallback().Register(OnGameControllerConnected);
		LeviathanInput::PlatformInput::GetGameControllerDisconnectedCallback().Register(OnGameControllerDisconnected);

		return true;
	}
}