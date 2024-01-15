#pragma once

namespace LeviathanRenderer
{
	class RenderContextInstance;

	namespace RenderDevice
	{
		namespace RenderCommands
		{
			bool BeginFrame();
			bool EndFrame();
		}

		bool Initialize();
		bool Shutdown();
		RenderContextInstance* CreateRenderContextInstance();
		void DestroyRenderContextInstance(RenderContextInstance* const context);
		bool InitializeRenderContextInstance(RenderContextInstance* const context);
		bool ShutdownRenderContextInstance(RenderContextInstance* const context);
	}
}
