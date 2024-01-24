#pragma once

#include "RenderContextSettings.h"

namespace LeviathanRenderer
{
	class RenderContextInstance;

	namespace RenderContext
	{
		RenderContextSettings& GetRenderContextSettings(RenderContextInstance* context);
	}
}