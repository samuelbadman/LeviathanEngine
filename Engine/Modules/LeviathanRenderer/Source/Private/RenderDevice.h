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
		bool BeginFrameCommandRecording(RenderContextInstance* const context);
		bool EndFrameCommandRecording(RenderContextInstance* const context);
		bool SubmitRecordedFrameCommands(RenderContextInstance* const context);
		bool Present(RenderContextInstance* const context);

		namespace RenderCommands
		{

		}
	}
}
