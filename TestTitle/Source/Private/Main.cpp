#include "LeviathanCore.h"

#ifdef LVTHN_BUILD_WIN32
int WINAPI wWinMain([[maybe_unused]] _In_ HINSTANCE hInstance, [[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance, [[maybe_unused]] _In_ PWSTR pCmdLine, [[maybe_unused]] _In_ int nCmdShow)
{
	return 0;
}
#else
int main()
{
	LeviathanCore::PrintHelloWorld();
	std::cin.get();
	return 0;
}
#endif
