#include "LeviathanCore.h"
#include "TestTitle.h"

static bool InitializeTitleModule()
{
	return TestTitle::Initialize();
}

LEVIATHAN_ENTRY_PROTOTYPE
{
	// Call engine core pre module initialization event.
	LeviathanCore::Core::PreModuleInitialization();

	// Initialize title module.
	if (!InitializeTitleModule())
	{
		return 1;
	}

	// Call engine core post module initialization event.
	LeviathanCore::Core::PostModuleInitialization();

	// Run engine.
	return LeviathanCore::Core::RunEngine();
}
