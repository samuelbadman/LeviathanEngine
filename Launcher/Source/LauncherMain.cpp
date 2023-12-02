#include "LeviathanCore.h"
#include "TestTitle.h"

LEVIATHAN_ENTRY_PROTOTYPE
{
	// Call on title to register engine callbacks.
	TestTitle::Setup();

	// Run engine.
	return LeviathanCore::Core::RunEngine();
}
