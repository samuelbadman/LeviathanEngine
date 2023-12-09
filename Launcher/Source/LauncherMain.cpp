#include "LeviathanCore.h"
#include "LeviathanInput.h"
#include "TestTitle.h"

LEVIATHAN_ENTRY_PROTOTYPE
{
	// Initialize title module.
	if (!TestTitle::Initialize())
	{
		return 1;
	}

	// Run engine.
	return LeviathanCore::Core::RunEngine();
}