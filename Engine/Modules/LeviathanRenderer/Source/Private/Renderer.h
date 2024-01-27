#pragma once

namespace LeviathanRenderer
{
	namespace Renderer
	{
		bool InitializeRendererApi(unsigned int width, unsigned int height, unsigned int verticalRefreshRate, void* windowPlatformHandle, bool vsync, unsigned int bufferCount);
		void Clear(const float* clearColor, float clearDepth, unsigned char clearStencil);
		void Present();
	}
}