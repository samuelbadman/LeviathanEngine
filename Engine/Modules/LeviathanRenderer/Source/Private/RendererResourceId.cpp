#include "RendererResourceId.h"

namespace LeviathanRenderer
{
	namespace RendererResourceId
	{
		static IdType CurrentId = 1;

		IdType GetAvailableId()
		{
			return CurrentId++;
		}
	}
}