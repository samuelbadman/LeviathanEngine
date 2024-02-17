#pragma once

namespace LeviathanRenderer
{
	namespace Renderer
	{
		[[nodiscard]] bool InitializeRendererApi(unsigned int width, unsigned int height, void* windowPlatformHandle, bool vsync, unsigned int bufferCount);
		[[nodiscard]] bool ShutdownRendererApi();
		[[nodiscard]] bool ResizeWindowResources(unsigned int width, unsigned int height);

		void Clear(const float* clearColor, float clearDepth, unsigned char clearStencil);
		void Present();
	}
}