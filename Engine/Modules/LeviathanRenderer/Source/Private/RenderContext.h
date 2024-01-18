#pragma once

namespace LeviathanRenderer
{
	class RenderContextInstance;

	namespace RenderContext
	{
		void SetVSyncEnabled(RenderContextInstance* const context, const bool vsyncEnabled);
		void SetSwapChainBackBufferCount(RenderContextInstance* const context, const unsigned int count);

		// Used to apply updated render context settings after the render context has been initialized.
		bool ApplyRenderContextSettings(RenderContextInstance* const context);
	}
}