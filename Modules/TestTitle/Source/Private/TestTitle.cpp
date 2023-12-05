#include "TestTitle.h"
#include "Core.h"

namespace TestTitle
{
	bool Initialize()
	{
		LeviathanCore::Core::GetPreMainLoopCallback().Register(OnPreMainLoop);
		LeviathanCore::Core::GetPostMainLoopCallback().Register(OnPostMainLoop);
		LeviathanCore::Core::GetTickCallback().Register(OnTick);
		LeviathanCore::Core::GetCleanupCallback().Register(OnCleanup);

		return true;
	}

	void OnPreMainLoop()
	{
		//std::cout << "Test title pre main loop.\n";
	}

	void OnPostMainLoop()
	{
		//std::cout << "Test title post main loop.\n";
	}

	void OnTick([[maybe_unused]] float DeltaSeconds)
	{
		//std::cout << "Tick: Delta seconds " << DeltaSeconds << '\n';
	}

	void OnCleanup()
	{
		std::cout << "Cleanup.\n";
	}
}