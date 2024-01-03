#pragma once

namespace LeviathanCore
{
	namespace String
	{
		std::string Printf(const char* format, ...);
	}
}

#define LEVIATHAN_STRING(format, ...) LeviathanCore::String::Printf(format, __VA_ARGS__)