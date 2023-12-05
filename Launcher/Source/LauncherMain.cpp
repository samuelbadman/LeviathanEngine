#include "LeviathanCore.h"
#include "TestTitle.h"

LEVIATHAN_ENTRY_PROTOTYPE
{
	// Initialize engine modules.
	TestTitle::Initialize();

	// Run engine.
	return LeviathanCore::Core::RunEngine();
}
