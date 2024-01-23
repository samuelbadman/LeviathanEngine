#pragma once

namespace LeviathanRenderer
{
	class RenderContextInstance;

	namespace RenderDevice
	{
		bool Initialize();
		bool Shutdown();
		RenderContextInstance* CreateRenderContextInstance();
		void DestroyRenderContextInstance(RenderContextInstance* const context);
		bool InitializeRenderContextInstance(RenderContextInstance* const context);
		bool ShutdownRenderContextInstance(RenderContextInstance* const context);
		bool BeginFrame(RenderContextInstance* const context);
		bool EndFrame(RenderContextInstance* const context);
		bool SubmitFrame(RenderContextInstance* const context);
		bool PresentFrame(RenderContextInstance* const context);
		bool ResizeRenderContextInstance(RenderContextInstance* const context);

		namespace RenderCommands
		{

		}
	}
}
