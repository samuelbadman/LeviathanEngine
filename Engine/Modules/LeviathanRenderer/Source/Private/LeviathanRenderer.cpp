#include "LeviathanRenderer.h"
#include "RenderDevice.h"
#include "RenderContext.h"

namespace LeviathanRenderer
{
	static RenderContextInstance* RuntimeWindowRenderContext = nullptr;

	static bool CreateAndInitializeRuntimeWindowRenderContext()
	{
		RuntimeWindowRenderContext = RenderDevice::CreateRenderContextInstance();

		RenderContext::SetVSyncEnabled(RuntimeWindowRenderContext, false);
		RenderContext::SetBackingBufferCount(RuntimeWindowRenderContext, 3);
		return RenderDevice::InitializeRenderContextInstance(RuntimeWindowRenderContext);
	}

	static bool ShutdownAndDestroyRuntimeWindowRenderContext()
	{
		if (!RenderDevice::ShutdownRenderContextInstance(RuntimeWindowRenderContext))
		{
			return false;
		}

		RenderDevice::DestroyRenderContextInstance(RuntimeWindowRenderContext);
		return true;
	}

	bool Initialize()
	{
		// Initialize render device.
		if (!RenderDevice::Initialize())
		{
			return false;
		}

		// Create and initialize render context for runtime window.
		if (!CreateAndInitializeRuntimeWindowRenderContext())
		{
			return false;
		}

		return true;
	}

	bool Shutdown()
	{
		// Shutdown and destroy render context for runtime window.
		if (!ShutdownAndDestroyRuntimeWindowRenderContext())
		{
			return false;
		}

		// Shutdown render device.
		if (!RenderDevice::Shutdown())
		{
			return false;
		}

		return true;
	}

	bool Render()
	{
		if (!RenderDevice::RenderCommands::BeginFrame())
		{
			return false;
		}

		if (!RenderDevice::RenderCommands::EndFrame())
		{
			return false;
		}

		return true;
	}
}
