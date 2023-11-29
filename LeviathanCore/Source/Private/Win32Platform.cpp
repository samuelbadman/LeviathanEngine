#include "Platform.h"

// Platform state.
static bool running = false;

enum class Win32FunctionErrors : unsigned int
{
	Success = 0,
	AllocConsoleFailed,
	FreeConsoleFailed,
	NullConsoleWindow,
	NullConsoleMenu,
	DeleteMenuFailed,
	ErrorMax
};

// Allocates a new console for the calling process. Returns the function result error code.
[[maybe_unused]] static Win32FunctionErrors AllocWinConsole()
{
	// Allocate console.
	if (!AllocConsole()) return Win32FunctionErrors::AllocConsoleFailed;

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

// Detaches the calling process from its console. Returns the function result error code.
[[maybe_unused]] static Win32FunctionErrors FreeWinConsole()
{
	// Detach from console.
	if (!FreeConsole()) return Win32FunctionErrors::FreeConsoleFailed;

	// Redirect standard IO.
	FILE* file;

	if (!(freopen_s(&file, "NUL:", "r", stdin) != 0)) setvbuf(stdin, nullptr, _IONBF, 0);
	if (!(freopen_s(&file, "NUL:", "w", stdout) != 0)) setvbuf(stdout, nullptr, _IONBF, 0);
	if (!(freopen_s(&file, "NUL:", "w", stderr) != 0)) setvbuf(stderr, nullptr, _IONBF, 0);

	std::ios::sync_with_stdio(false);

	return Win32FunctionErrors::Success;
}

// Disables the top right close button in the console window. Returns the function result error code.
[[maybe_unused]] static Win32FunctionErrors DisableConsoleCloseButton()
{
	HWND consoleWndHandle = GetConsoleWindow();
	if (!consoleWndHandle) return Win32FunctionErrors::NullConsoleWindow;

	HMENU consoleMenu = GetSystemMenu(consoleWndHandle, FALSE);
	if (!consoleMenu) return Win32FunctionErrors::NullConsoleMenu;

	if (!DeleteMenu(consoleMenu, SC_CLOSE, MF_BYCOMMAND)) return Win32FunctionErrors::DeleteMenuFailed;

	return Win32FunctionErrors::Success;
}

unsigned char LeviathanCore::Platform::LeviathanEntry()
{
	if (AllocWinConsole() != Win32FunctionErrors::Success) return 1;
	if (DisableConsoleCloseButton() != Win32FunctionErrors::Success) return 1;

	while (true)
	{

	}

	if (FreeWinConsole() != Win32FunctionErrors::Success) return 1;

	return 0;
}

void LeviathanCore::Platform::Exit()
{
	running = false;
}
