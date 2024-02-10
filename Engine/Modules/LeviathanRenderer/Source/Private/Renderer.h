#pragma once

namespace LeviathanRenderer
{
	namespace Renderer
	{
		void InitializeRendererApi(unsigned int width, unsigned int height, void* windowPlatformHandle, bool vsync, unsigned int bufferCount);
		void ResizeWindowResources(unsigned int width, unsigned int height);

		void Clear(const float* clearColor, float clearDepth, unsigned char clearStencil);
		void Present();
	}
}