#include "LeviathanRenderer.h"
#include "RenderDevice.h"

bool LeviathanRenderer::Initialize()
{
	// Initialize render device.
	if (!RenderDevice::Initialize())
	{
		return false;
	}

	// Create render context for runtime window.



	return true;
}

bool LeviathanRenderer::Shutdown()
{
	return true;
}
