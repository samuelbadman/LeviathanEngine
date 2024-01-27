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
		// Register to runtime window callbacks.
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Register(&OnRuntimeWindowResized);

		int width = 0;
		int height = 0;
		if (!LeviathanCore::Core::GetRuntimeWindowRenderAreaDimensions(width, height))
		{
			return false;
		}

		LeviathanCore::Platform::Displays::DisplayDetails display = LeviathanCore::Platform::Displays::GetDisplayDetails(0);

		void* platformHandle = LeviathanCore::Core::GetRuntimeWindowPlatformHandle();

		static constexpr unsigned int bufferCount = 3;

		if (!Renderer::InitializeRendererApi(static_cast<unsigned int>(width),
			static_cast<unsigned int>(height), 
			display.VerticalRefreshRateHertz, 
			platformHandle, 
			false,
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
}
