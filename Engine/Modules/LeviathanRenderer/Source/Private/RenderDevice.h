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

		namespace RenderCommands
		{
			// Begin a render frame for the render context.
			bool CmdBeginFrame(RenderContextInstance* const context);

			// End a render frame for the render context.
			bool CmdEndFrame(RenderContextInstance* const context);
		}
	}
}
