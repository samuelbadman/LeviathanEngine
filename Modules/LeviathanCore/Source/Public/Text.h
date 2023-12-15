#pragma once

namespace LeviathanCore
{
	namespace Text
	{
		std::string Printf(const char* format, ...);
	}
}

#define LEVIATHAN_TEXT(format, ...) LeviathanCore::Text::Printf(format, __VA_ARGS__)