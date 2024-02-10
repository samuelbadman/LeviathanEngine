#include "LeviathanRenderer.h"
#include "Logging.h"
#include "Core.h"
#include "Renderer.h"

namespace LeviathanRenderer
{
	static void OnRuntimeWindowResized(int newWidth, int newHeight)
	{
		Renderer::ResizeWindowResources(newWidth, newHeight);
	}

	bool Initialize()
	{
		static constexpr unsigned int bufferCount = 3;
		static constexpr bool vsync = false;

		// Register to runtime window callbacks.
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Register(&OnRuntimeWindowResized);

		// Initialize renderer api.
		void* platformHandle = LeviathanCore::Core::GetRuntimeWindowPlatformHandle();
		int width = 0;
		int height = 0;

		if (!LeviathanCore::Core::GetRuntimeWindowRenderAreaDimensions(width, height))
		{
			return false;
		}

		Renderer::InitializeRendererApi(static_cast<unsigned int>(width), static_cast<unsigned int>(height), platformHandle, vsync, bufferCount);

		return true;
	}

	bool Shutdown()
	{
		return true;
	}

	bool Render()
	{
		static constexpr float clearColor[] = { 1.f, 0.f, 0.f, 1.f };
		static constexpr float clearDepth = 1.f;
		static constexpr unsigned char clearStencil = 0;

		Renderer::Clear(clearColor, clearDepth, clearStencil);
		Renderer::Present();

		return true;
	}
}
