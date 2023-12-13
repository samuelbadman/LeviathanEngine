#include "LeviathanInputCore.h"
#include "PlatformInput.h"

namespace LeviathanInputCore
{
	bool Initialize()
	{
		return PlatformInput::Initialize();
	}

	bool Shutdown()
	{
		return PlatformInput::Shutdown();
	}
}