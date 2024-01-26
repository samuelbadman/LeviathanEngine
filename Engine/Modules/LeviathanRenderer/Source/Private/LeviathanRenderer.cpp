#include "LeviathanRenderer.h"
#include "Logging.h"
#include "Core.h"

namespace LeviathanRenderer
{
	static void OnRuntimeWindowResized([[maybe_unused]] int newWidth, [[maybe_unused]] int newHeight)
	{

	}

	bool Initialize()
	{
		// Register to runtime window callbacks.
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Register(&OnRuntimeWindowResized);

		return true;
	}

	bool Shutdown()
	{
		return true;
	}
}
