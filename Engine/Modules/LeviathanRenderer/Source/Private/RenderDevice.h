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

		// Used to apply updated render context settings after the render context has been initialized.
		[[maybe_unused]] bool ApplyRenderContextSettings([[maybe_unused]] RenderContextInstance* const context);
	}
}
