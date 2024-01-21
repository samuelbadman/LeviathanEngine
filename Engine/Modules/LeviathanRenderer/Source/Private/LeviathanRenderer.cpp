#include "LeviathanRenderer.h"
#include "RenderDevice.h"
#include "RenderContext.h"
#include "Core.h"
#include "Logging.h"

namespace LeviathanRenderer
{
	static constexpr bool DefaultRenderContextVSyncEnabledState = true;
	static constexpr unsigned int RenderContextSwapChainBackBufferCount = 3;

	static RenderContextInstance* RuntimeWindowRenderContext = nullptr;

	static bool CreateAndInitializeRuntimeWindowRenderContext()
	{
		RuntimeWindowRenderContext = RenderDevice::CreateRenderContextInstance();

		// Set default render context state before intializing.
		RenderContext::SetVSyncEnabled(RuntimeWindowRenderContext, DefaultRenderContextVSyncEnabledState);
		RenderContext::SetSwapChainBackBufferCount(RuntimeWindowRenderContext, RenderContextSwapChainBackBufferCount);
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
		// Render runtime window render context.
		if (!LeviathanCore::Core::IsRuntimeWindowMinimized())
		{
			if (!RenderDevice::BeginFrameCommandRecording(RuntimeWindowRenderContext))
			{
				return false;
			}

			// TODO: Commands...

			if (!RenderDevice::EndFrameCommandRecording(RuntimeWindowRenderContext))
			{
				return false;
			}

			if (!RenderDevice::SubmitRecordedFrameCommands(RuntimeWindowRenderContext))
			{
				return false;
			}

			if (!RenderDevice::Present(RuntimeWindowRenderContext))
			{
				return false;
			}
		}

		return true;
	}
}
