#include "Logging.h"

namespace LeviathanCore
{
	namespace Logging
	{
		void PrintToStandardOutput(std::string_view string)
		{
			std::cout << string << '\n';
		}
	}
}