#include "Platform.h"

enum class Win32FunctionErrors : unsigned int
{
	Success = 0,
	AllocConsoleFailed,
	FreeConsoleFailed
};

// Allocates a new console for the calling process. Returns true if the function succeeds otherwise, 
// returns false if the function fails. Call GetLastError to get extended error information.
static Win32FunctionErrors AllocWinConsole()
{
	// Allocate console.
	if ((AllocConsole() == 0)) return Win32FunctionErrors::AllocConsoleFailed;

	// Redirect standard IO.
	FILE* file;

	if (!(freopen_s(&file, "CONOUT$", "w", stdout) != 0)) setvbuf(stdout, nullptr, _IONBF, 0);
	if (!(freopen_s(&file, "CONIN$", "r", stdin) != 0)) setvbuf(stdin, nullptr, _IONBF, 0);
	if (!(freopen_s(&file, "CONOUT$", "w", stderr) != 0)) setvbuf(stderr, nullptr, _IONBF, 0);

	std::ios::sync_with_stdio(true);
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();

	return Win32FunctionErrors::Success;
}

// Detaches the calling process from its console. Returns true if the function succeeds otherwise,
// returns false. Call GetLastError to get extended error information.
static Win32FunctionErrors FreeWinConsole()
{
	// Detach from console.
	if (FreeConsole() == 0) return Win32FunctionErrors::FreeConsoleFailed;

	// Redirect standard IO.
	FILE* file;

	if (!(freopen_s(&file, "NUL:", "r", stdin) != 0)) setvbuf(stdin, nullptr, _IONBF, 0);
	if (!(freopen_s(&file, "NUL:", "w", stdout) != 0)) setvbuf(stdout, nullptr, _IONBF, 0);
	if (!(freopen_s(&file, "NUL:", "w", stderr) != 0)) setvbuf(stderr, nullptr, _IONBF, 0);

	std::ios::sync_with_stdio(false);

	return Win32FunctionErrors::Success;
}

unsigned char LeviathanCore::Platform::LeviathanEntry()
{
	AllocWinConsole();
	FreeWinConsole();
	return 0;
}
