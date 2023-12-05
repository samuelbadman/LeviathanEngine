#include "LeviathanCore.h"
#include "LeviathanInput.h"
#include "TestTitle.h"

LEVIATHAN_ENTRY_PROTOTYPE
{
	// Initialize engine modules.
	if (!LeviathanInput::Initialize())
	{
		return 1;
	}

	if (!TestTitle::Initialize())
	{
		return 1;
	}

	// Run engine.
	return LeviathanCore::Core::RunEngine();
}
