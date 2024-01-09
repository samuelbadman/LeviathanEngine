#pragma once

namespace LeviathanRenderer
{
	class RenderContextInstance;

	namespace RenderContext
	{
		void SetVSyncEnabled(RenderContextInstance* const context, const bool vsyncEnabled);
		void SetBackingBufferCount(RenderContextInstance* const context, const unsigned int count);
		bool ApplyRenderContextSettings(RenderContextInstance* const context);
	}
}