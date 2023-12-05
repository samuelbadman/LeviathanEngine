#include "TestTitle.h"
#include "Core.h"

void TestTitle::Initialize()
{
	LeviathanCore::Core::GetPreMainLoopCallback().Register(OnPreMainLoop);
	LeviathanCore::Core::GetPostMainLoopCallback().Register(OnPostMainLoop);
	LeviathanCore::Core::GetTickCallback().Register(OnTick);
	LeviathanCore::Core::GetCleanupCallback().Register(OnCleanup);
}

void TestTitle::OnPreMainLoop()
{
	//std::cout << "Test title pre main loop.\n";
}

void TestTitle::OnPostMainLoop()
{
	//std::cout << "Test title post main loop.\n";
}

void TestTitle::OnTick([[maybe_unused]] float DeltaSeconds)
{
	//std::cout << "Tick: Delta seconds " << DeltaSeconds << '\n';
}

void TestTitle::OnCleanup()
{
	std::cout << "Cleanup.\n";
}
