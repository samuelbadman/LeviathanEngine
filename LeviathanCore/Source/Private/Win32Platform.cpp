#include "Platform.h"

namespace LeviathanCore
{
	namespace Platform
	{
		static LARGE_INTEGER TicksPerSecond = {};
		static LARGE_INTEGER LastTickCount = {};
		static unsigned long long ElapsedMicroseconds = 0;

		bool CreateDebugConsole()
		{
			// Allocate console.
			if (!AllocConsole())
			{
				// AllocConsole failed.
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

			// Console creation succeeded.
			return true;
		}

		bool DestroyDebugConsole()
		{
			// Detach from console.
			if (!FreeConsole())
			{
				// FreeConsole failed.
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

			// Console destruction succeeded.
			return true;
		}

		bool InitializeTiming()
		{
			if (!QueryPerformanceFrequency(&TicksPerSecond))
			{
				return false;
			}

			if (!QueryPerformanceCounter(&LastTickCount))
			{
				return false;
			}

			return true;
		}

		bool UpdateDeltaTime()
		{
			LARGE_INTEGER currentTickCount = {};
			if (QueryPerformanceCounter(&currentTickCount))
			{
				const uint64_t elapsedTicks = currentTickCount.QuadPart - LastTickCount.QuadPart;

				// Convert to microseconds to not lose precision, by dividing a small number by a large one.
				ElapsedMicroseconds = (elapsedTicks * static_cast<unsigned long long>(1e6)) / TicksPerSecond.QuadPart;

				LastTickCount = currentTickCount;

				return true;
			}

			return false;
		}

		float GetDeltaTimeInMilliseconds()
		{
			return static_cast<float>(ElapsedMicroseconds) * 1e-3f;
		}

		float GetDeltaTimeInSeconds()
		{
			return GetDeltaTimeInMilliseconds() * 1e-3f;
		}
	}
}