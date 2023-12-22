#pragma once

#include "Text.h"

namespace LeviathanCore
{
	namespace Logging
	{
		// Sends the string to standard output.
		void PrintToStandardOutput(std::string_view string);
	}
}

// Logging macros enabling stripping of log functionality for master build configuration.
#ifndef LEVIATHAN_BUILD_CONFIG_MASTER
#define LEVIATHAN_LOG(format, ...) LeviathanCore::Logging::PrintToStandardOutput(LEVIATHAN_TEXT(format, __VA_ARGS__))
#else
#define LEVIATHAN_LOG(format, ...)
#endif // !LEVIATHAN_BUILD_CONFIG_MASTER
