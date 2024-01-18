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
			bool CmdBeginFrame(VkCommandBuffer commandBuffer);
		}
	}
}
