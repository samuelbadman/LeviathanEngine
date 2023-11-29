#include "Platform.h"
#include "Macros/BitMacros.h"
#include "Platform/Win32/Win32FunctionErrors.h"

static bool running = false;
static bool restart = false;

// Allocates a new console for the calling process. Returns the function result error code.
[[maybe_unused]] static Win32FunctionError AllocWinConsole()
{
	// Allocate console.
	if (!AllocConsole())
	{
		return Win32FunctionError::AllocConsoleFailed;
	}

	// Redirect standard IO.
	FILE* file;

	if (!(freopen_s(&file, "CONOUT$", "w", stdout) != 0))
	{
		setvbuf(stdout, nullptr, _IONBF, 0);
	}

	if (!(freopen_s(&file, "CONIN$", "r", stdin) != 0))
	{
		setvbuf(stdin, nullptr, _IONBF, 0);
	}

	if (!(freopen_s(&file, "CONOUT$", "w", stderr) != 0))
	{
		setvbuf(stderr, nullptr, _IONBF, 0);
	}

	std::ios::sync_with_stdio(true);
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();

	return Win32FunctionError::Success;
}

// Detaches the calling process from its console. Returns the function result error code.
[[maybe_unused]] static Win32FunctionError FreeWinConsole()
{
	// Detach from console.
	if (!FreeConsole())
	{
		return Win32FunctionError::FreeConsoleFailed;
	}

	// Redirect standard IO.
	FILE* file;

	if (!(freopen_s(&file, "NUL:", "r", stdin) != 0))
	{
		setvbuf(stdin, nullptr, _IONBF, 0);
	}

	if (!(freopen_s(&file, "NUL:", "w", stdout) != 0))
	{
		setvbuf(stdout, nullptr, _IONBF, 0);
	}

	if (!(freopen_s(&file, "NUL:", "w", stderr) != 0))
	{
		setvbuf(stderr, nullptr, _IONBF, 0);
	}

	std::ios::sync_with_stdio(false);

	return Win32FunctionError::Success;
}

unsigned char LeviathanCore::Platform::LeviathanEntry()
{
	if (AllocWinConsole() != Win32FunctionError::Success)
	{
		return 1;
	}

	while (true)
	{
		running = true;

		while (running)
		{

		}

		if (!restart)
		{
			break;
		}
	}

	if (FreeWinConsole() != Win32FunctionError::Success)
	{
		return 1;
	}

	return 0;
}

void LeviathanCore::Platform::Exit(bool Restart)
{
	running = false;
	restart = Restart;
}
