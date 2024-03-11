#include "MathLibrary.h"

namespace LeviathanCore
{
	namespace MathLibrary
	{
		float WrapAngle(const float angleRadians)
		{
			return angleRadians - TwoPi * std::floor(angleRadians / TwoPi);
		}
	}
}


