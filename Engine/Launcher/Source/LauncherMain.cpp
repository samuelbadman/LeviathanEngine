#include "LeviathanCore.h"
#include "TestTitle.h"

static bool InitializeTitleModule()
{
	return TestTitle::Initialize();
}

LEVIATHAN_ENTRY_PROTOTYPE
{
	// TODO: Wrap this inside a while loop that loops if RunEngine returns that the engine was exited with a signal to restart. 
	// Consider making core a class that can be constructed and destroyed to ensure resource initialization is correct.
	bool restart = true;
	int error = 0;

	while (restart)
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
		error = LeviathanCore::Core::RunEngine(restart);
	}

	return error;
}
