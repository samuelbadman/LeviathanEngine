#include "RendererResourceId.h"

namespace LeviathanRenderer
{
	namespace RendererResourceID
	{
		static IDType CurrentID = 1;

		IDType GetAvailableID()
		{
			return CurrentID++;
		}
	}
}