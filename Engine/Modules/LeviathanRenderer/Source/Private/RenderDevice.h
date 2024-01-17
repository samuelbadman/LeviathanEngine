#pragma once

namespace LeviathanRenderer
{
	class RenderContextInstance;

	namespace RenderDevice
	{
		bool Initialize(const unsigned int backBufferCount);
		bool Shutdown();
		RenderContextInstance* CreateRenderContextInstance();
		void DestroyRenderContextInstance(RenderContextInstance* const context);
		bool InitializeRenderContextInstance(RenderContextInstance* const context);
		bool ShutdownRenderContextInstance(RenderContextInstance* const context);
	}
}
