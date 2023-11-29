#include "Platform.h"
#include "Macros/BitMacros.h"
#include "Core/Core.h"

// Allocates a new console for the calling process. Returns true if the function succeeds otherwise returns false.
[[maybe_unused]] static bool AllocWinConsole()
{
	// Allocate console.
	if (!AllocConsole())
	{
		return false;
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

	return true;
}

// Detaches the calling process from its console. Returns true if the function succeeds otherwise returns false.
[[maybe_unused]] static bool FreeWinConsole()
{
	// Detach from console.
	if (!FreeConsole())
	{
		return false;
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

	return true;
}

struct Win32EntryState
{
	bool restart = true;
	bool running = false;
	LARGE_INTEGER ticksPerSecond = {};
	LARGE_INTEGER lastTickCount = {};
	float deltaSeconds = 0.0f;
};

static Win32EntryState state = {};

static bool InitializeDeltaTimeCount()
{
	return ((QueryPerformanceFrequency(&state.ticksPerSecond)) && (QueryPerformanceCounter(&state.lastTickCount)));
}

static bool UpdateDeltaTimeCount()
{
	LARGE_INTEGER currentTickCount = {};
	if (QueryPerformanceCounter(&currentTickCount) != 0)
	{
		const uint64_t elapsedTicks = currentTickCount.QuadPart - state.lastTickCount.QuadPart;

		// Convert to microseconds to not lose precision, by dividing a small number by a large one.
		const uint64_t elapsedTimeInMicroseconds = (elapsedTicks * static_cast<uint64_t>(1e6)) / state.ticksPerSecond.QuadPart;

		// Time in milliseconds.
		const float deltaMilliseconds = static_cast<float>(elapsedTimeInMicroseconds) * 1e-3f;

		// Time in seconds.
		state.deltaSeconds = deltaMilliseconds * 1e-3f;

		state.lastTickCount = currentTickCount;

		return true;
	}

	return false;
}

unsigned char LeviathanCore::Platform::LeviathanEntry()
{
	if (!AllocWinConsole())
	{
		return 1;
	}

	while (state.restart)
	{
		state =
		{
			.restart = false,
			.running = true
		};

		if (!InitializeDeltaTimeCount())
		{
			return 1;
		}

		if (!Core::Initialize())
		{
			return 1;
		}

		while (state.running)
		{
			UpdateDeltaTimeCount();
			// TODO: Pump system message queue.
			// TODO: Pump input message queue.
			Core::MainLoopIteration(state.deltaSeconds);
		}

		if (!Core::Shutdown())
		{
			return 1;
		}
	}

	if (!FreeWinConsole())
	{
		return 1;
	}

	return 0;
}

void LeviathanCore::Platform::Exit(bool Restart)
{
	state.running = false;
	state.restart = Restart;
}
