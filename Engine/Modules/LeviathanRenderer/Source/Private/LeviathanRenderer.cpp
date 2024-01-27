#include "LeviathanRenderer.h"
#include "Logging.h"
#include "Core.h"
#include "Platform.h"
#include "Renderer.h"

namespace LeviathanRenderer
{
	static void OnRuntimeWindowResized([[maybe_unused]] int newWidth, [[maybe_unused]] int newHeight)
	{

	}

	bool Initialize()
	{
		static constexpr unsigned int bufferCount = 3;
		static constexpr bool vsync = false;

		// Register to runtime window callbacks.
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Register(&OnRuntimeWindowResized);

		// Initialize renderer api.
		int width = 0;
		int height = 0;
		if (!LeviathanCore::Core::GetRuntimeWindowRenderAreaDimensions(width, height))
		{
			return false;
		}

		LeviathanCore::Platform::Displays::DisplayDetails display = LeviathanCore::Platform::Displays::GetDisplayDetails(0);

		void* platformHandle = LeviathanCore::Core::GetRuntimeWindowPlatformHandle();

		if (!Renderer::InitializeRendererApi(static_cast<unsigned int>(width),
			static_cast<unsigned int>(height), 
			display.VerticalRefreshRateHertz, 
			platformHandle, 
			vsync,
			bufferCount))
		{
			return false;
		}

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
