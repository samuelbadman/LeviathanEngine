#include "RendererResourceId.h"

namespace LeviathanRenderer
{
	namespace RendererResourceId
	{
		static IdType CurrentID = 1;

		IdType GetAvailableId()
		{
			return CurrentID++;
		}
	}
}