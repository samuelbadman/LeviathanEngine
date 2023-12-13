#include "LeviathanCore.h"
#include "LeviathanInputCore.h"
#include "TestTitle.h"

static bool InitializeTitleModule()
{
	if (!TestTitle::Initialize())
	{
		return false;
	}

	return true;
}

LEVIATHAN_ENTRY_PROTOTYPE
{
	// Initialize title module.
	if (!InitializeTitleModule())
	{
		return 1;
	}

	// Run engine.
	return LeviathanCore::Core::RunEngine();
}
