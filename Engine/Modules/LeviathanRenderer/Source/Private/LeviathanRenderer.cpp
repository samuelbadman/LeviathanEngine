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
	// Destroy render context for runtime window.


	// Shutdown render device.
	if (!RenderDevice::Shutdown())
	{
		return false;
	}

	return true;
}
