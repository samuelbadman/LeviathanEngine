#include "LeviathanRenderer.h"
#include "RenderDevice.h"
#include "RenderContext.h"

namespace LeviathanRenderer
{
	// The number of back buffers created for the swap chains in render contexts. Also, controls the number of command list/buffer resources that are created within the render device.
	static constexpr unsigned int RendererBackBufferCount = 3;

	static RenderContextInstance* RuntimeWindowRenderContext = nullptr;

	static bool CreateAndInitializeRuntimeWindowRenderContext()
	{
		RuntimeWindowRenderContext = RenderDevice::CreateRenderContextInstance();

		// Set default render context state before intializing.
		RenderContext::SetVSyncEnabled(RuntimeWindowRenderContext, true);
		RenderContext::SetBackBufferCount(RuntimeWindowRenderContext, RendererBackBufferCount);
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
		if (!RenderDevice::Initialize(RendererBackBufferCount))
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


		return true;
	}
}
