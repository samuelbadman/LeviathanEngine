#include "MathLibrary.h"

namespace LeviathanCore
{
	namespace MathLibrary
	{
		float WrapAngleRadians(const float angleRadians)
		{
			return angleRadians - TwoPi * std::floor(angleRadians / TwoPi);
		}
	}
}


