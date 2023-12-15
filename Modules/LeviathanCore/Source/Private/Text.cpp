#include "Text.h"

namespace LeviathanCore
{
	namespace Text
	{
		std::string Printf(const char* format, ...)
		{
			va_list args;
			va_start(args, format);

			// Convert format string to a string. Requires heap allocation.
			size_t size = vsnprintf(nullptr, 0, format, args) + 1;
			std::string buffer;
			buffer.resize(size);
			vsnprintf(buffer.data(), size, format, args);
			buffer[size - 1] = '\0';

			// Print string created from formatted string.
			//std::cout << buffer << '\n';

			// Just print formatted string.
			//vprintf(format, args);

			va_end(args);

			// Return string
			return buffer;
		}
	}
}