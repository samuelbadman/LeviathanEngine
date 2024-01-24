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

	static void OnRuntimeWindowResized([[maybe_unused]] int newWidth, [[maybe_unused]] int newHeight)
	{
		[[maybe_unused]] bool success = RenderDevice::ResizeRenderContextInstance(RuntimeWindowRenderContext);

		LEVIATHAN_LOG("Resize runtime window render context result: %s.", ((success) ? "Success" : "Failed"));
	}

	static bool CreateAndInitializeRuntimeWindowRenderContext()
	{
		RuntimeWindowRenderContext = RenderDevice::CreateRenderContextInstance();

		// Set default render context state before intializing.
		RenderContextSettings& RuntimeWindowRenderContextSettings = RenderContext::GetRenderContextSettings(RuntimeWindowRenderContext);
		RuntimeWindowRenderContextSettings.VSyncEnabled = DefaultRenderContextVSyncEnabledState;
		RuntimeWindowRenderContextSettings.SwapChainBackBufferCount = RenderContextSwapChainBackBufferCount;

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
		// Register to runtime window callbacks.
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Register(&OnRuntimeWindowResized);

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
		if (!RenderDevice::BeginFrame(RuntimeWindowRenderContext))
		{
			return false;
		}

		// TODO: Commands...

		if (!RenderDevice::EndFrame(RuntimeWindowRenderContext))
		{
			return false;
		}

		if (!RenderDevice::SubmitFrame(RuntimeWindowRenderContext))
		{
			return false;
		}

		if (!RenderDevice::PresentFrame(RuntimeWindowRenderContext))
		{
			return false;
		}

		return true;
	}
}
