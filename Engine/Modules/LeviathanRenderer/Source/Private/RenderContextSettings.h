#pragma once

namespace LeviathanRenderer
{
	struct RenderContextSettings
	{
		bool VSyncEnabled = true;
		unsigned int SwapChainBackBufferCount = 3;
	};
}