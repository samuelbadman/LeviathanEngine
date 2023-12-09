#include "TestTitle.h"
#include "LeviathanCore.h"
#include "LeviathanInput.h"

namespace TestTitle
{
	static bool Shutdown()
	{
		// Shutdown engine modules used by title.
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

	}

	static void OnPostTick()
	{

	}

	static void OnCleanup()
	{
		Shutdown();
	}

	static void OnInput([[maybe_unused]] LeviathanCore::InputKey key, [[maybe_unused]] bool isRepeatKey, [[maybe_unused]] float data)
	{

	}

	static void OnGameControllerInput([[maybe_unused]] LeviathanCore::InputKey key, [[maybe_unused]] bool isRepeatKey, [[maybe_unused]] float data, [[maybe_unused]] unsigned int gameControllerId)
	{

	}

	static void OnGameControllerConnected([[maybe_unused]] unsigned int gameControllerId)
	{

	}

	static void OnGameControllerDisconnected([[maybe_unused]] unsigned int gameControllerId)
	{

	}

	bool Initialize()
	{
		// Initialize engine modules for title.
		if (!LeviathanInput::Initialize())
		{
			return false;
		}

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