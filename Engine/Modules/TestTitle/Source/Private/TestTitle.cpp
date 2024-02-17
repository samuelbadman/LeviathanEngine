#include "TestTitle.h"
#include "LeviathanCore.h"
#include "LeviathanInputCore.h"
#include "LeviathanRenderer.h"

namespace TestTitle
{
	static bool Shutdown()
	{
		// Shutdown engine modules used by title.
		if (!LeviathanInputCore::Shutdown())
		{
			return false;
		}

		if (!LeviathanRenderer::Shutdown())
		{
			return false;
		}

		// Deregister callbacks.
		LeviathanCore::Core::GetCleanupCallback().Deregister(&OnCleanup);
		LeviathanCore::Core::GetPreMainLoopCallback().Deregister(&OnPreMainLoop);
		LeviathanCore::Core::GetPostMainLoopCallback().Deregister(&OnPostMainLoop);
		LeviathanCore::Core::GetPreTickCallback().Deregister(&OnPreTick);
		LeviathanCore::Core::GetFixedTickCallback().Deregister(&OnFixedTick);
		LeviathanCore::Core::GetTickCallback().Deregister(&OnTick);
		LeviathanCore::Core::GetPostTickCallback().Deregister(&OnPostTick);
		LeviathanCore::Core::GetRenderCallback().Deregister(&OnRender);

		LeviathanInputCore::PlatformInput::GetInputCallback().Deregister(&OnInput);
		LeviathanInputCore::PlatformInput::GetGameControllerInputCallback().Deregister(&OnGameControllerInput);
		LeviathanInputCore::PlatformInput::GetGameControllerConnectedCallback().Deregister(&OnGameControllerConnected);
		LeviathanInputCore::PlatformInput::GetGameControllerDisconnectedCallback().Deregister(&OnGameControllerDisconnected);

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

	static void OnFixedTick([[maybe_unused]] float fixedTimestep)
	{

	}

	static void OnTick([[maybe_unused]] float deltaSeconds)
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

	static void OnRender()
	{
		LeviathanRenderer::RenderFrame();
	}

	bool Initialize()
	{
		// Initialize engine modules for title.
		if (!LeviathanInputCore::Initialize())
		{
			return false;
		}

		if (!LeviathanRenderer::Initialize())
		{
			return false;
		}

		// Register callbacks.
		LeviathanCore::Core::GetCleanupCallback().Register(&OnCleanup);
		LeviathanCore::Core::GetPreMainLoopCallback().Register(&OnPreMainLoop);
		LeviathanCore::Core::GetPostMainLoopCallback().Register(&OnPostMainLoop);
		LeviathanCore::Core::GetPreTickCallback().Register(&OnPreTick);
		LeviathanCore::Core::GetFixedTickCallback().Register(&OnFixedTick);
		LeviathanCore::Core::GetTickCallback().Register(&OnTick);
		LeviathanCore::Core::GetPostTickCallback().Register(&OnPostTick);
		LeviathanCore::Core::GetRenderCallback().Register(&OnRender);

		LeviathanInputCore::PlatformInput::GetInputCallback().Register(&OnInput);
		LeviathanInputCore::PlatformInput::GetGameControllerInputCallback().Register(&OnGameControllerInput);
		LeviathanInputCore::PlatformInput::GetGameControllerConnectedCallback().Register(&OnGameControllerConnected);
		LeviathanInputCore::PlatformInput::GetGameControllerDisconnectedCallback().Register(&OnGameControllerDisconnected);

		return true;
	}
}