#include "TestTitle.h"
#include "Core.h"

void TestTitle::Setup()
{
	LeviathanCore::Core::PreMainLoopCallback.Register(OnPreMainLoop);
	LeviathanCore::Core::PostMainLoopCallback.Register(OnPostMainLoop);
}

void TestTitle::OnPreMainLoop()
{
	std::cout << "Test title pre main loop.\n";
}

void TestTitle::OnPostMainLoop()
{
	std::cout << "Test title post main loop.\n";
}
