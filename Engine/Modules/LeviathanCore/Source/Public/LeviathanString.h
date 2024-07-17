#pragma once

namespace LeviathanCore
{
	namespace String
	{
		std::string Printf(const char* format, ...);

		template<typename T>
		std::string AsString(const T value)
		{
			return std::to_string(value);
		}
	}
}
