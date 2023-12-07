#include "LeviathanInput.h"
#include "PlatformInput.h"

namespace LeviathanInput
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