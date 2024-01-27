#pragma once

namespace LeviathanRenderer
{
	namespace Renderer
	{
		bool InitializeRendererApi(unsigned int width, unsigned int height, unsigned int verticalRefreshRate, void* windowPlatformHandle, bool vsync, unsigned int bufferCount);
	}
}