#include "LeviathanRenderer.h"
#include "Logging.h"
#include "Core.h"
#include "Renderer.h"

namespace LeviathanRenderer
{
#ifdef LEVIATHAN_WITH_TOOLS
	static LeviathanCore::Callback<RenderImGuiCallbackType> RenderImGuiCallback = {};
#endif // LEVIATHAN_WITH_TOOLS.

	static void OnRuntimeWindowResized(int renderAreaWidth, int renderAreaHeight)
	{
		bool success = Renderer::ResizeWindowResources(renderAreaWidth, renderAreaHeight);

		if (!success)
		{
			LEVIATHAN_LOG("Failed to resize runtime window renderer resources.");
		}
	}

#ifdef LEVIATHAN_WITH_TOOLS
	static void OnImGuiRendererNewFrame()
	{
		Renderer::ImGuiRendererNewFrame();
	}

	static void OnImGuiRender()
	{
		RenderImGuiCallback.Call();
		Renderer::ImGuiRender();
	}

	LeviathanCore::Callback<RenderImGuiCallbackType>& GetRenderImGuiCallback()
	{
		return RenderImGuiCallback;
	}
#endif // LEVIATHAN_WITH_TOOLS.

	bool Initialize()
	{
		static constexpr unsigned int bufferCount = 3;
		static constexpr bool vsync = false;

#ifdef LEVIATHAN_WITH_TOOLS
		// Clear render ImGui callback.
		RenderImGuiCallback.Clear();
#endif // LEVIATHAN_WITH_TOOLS.

		// Register to callbacks.
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Register(&OnRuntimeWindowResized);

#ifdef LEVIATHAN_WITH_TOOLS
		LeviathanCore::Core::GetImGuiRendererNewFrameCallback().Register(&OnImGuiRendererNewFrame);
		LeviathanCore::Core::GetImGuiRenderCallback().Register(&OnImGuiRender);
#endif // LEVIATHAN_WITH_TOOLS.

		// Initialize renderer api.
		void* platformHandle = LeviathanCore::Core::GetRuntimeWindowPlatformHandle();
		int width = 0;
		int height = 0;

		if (!LeviathanCore::Core::GetRuntimeWindowRenderAreaDimensions(width, height))
		{
			return false;
		}

		if (!Renderer::InitializeRendererApi(static_cast<unsigned int>(width), static_cast<unsigned int>(height), platformHandle, vsync, bufferCount))
		{
			return false;
		}

#ifdef LEVIATHAN_WITH_TOOLS
		if (!Renderer::ImGuiRendererInitialize())
		{
			return false;
		}
#endif // LEVIATHAN_WITH_TOOLS.

		return true;
	}

	bool Shutdown()
	{
		// Deregister from callbacks.
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Deregister(&OnRuntimeWindowResized);

#ifdef LEVIATHAN_WITH_TOOLS
		// Deregister from Leviathan tools callbacks.
		LeviathanCore::Core::GetImGuiRendererNewFrameCallback().Deregister(&OnImGuiRendererNewFrame);
		LeviathanCore::Core::GetImGuiRenderCallback().Deregister(&OnImGuiRender);

		// Shutdown Leviathan tools renderer.
		Renderer::ImGuiRendererShutdown();
#endif // LEVIATHAN_WITH_TOOLS.

		if (!Renderer::ShutdownRendererApi())
		{
			return false;
		}

		return true;
	}

	bool CreateVertexBuffer(const void* vertexData, unsigned int vertexCount, size_t singleVertexStrideBytes, RendererResourceID::IDType& outId)
	{
		return Renderer::CreateVertexBuffer(vertexData, vertexCount, singleVertexStrideBytes, outId);
	}

	bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceID::IDType& outId)
	{
		return Renderer::CreateIndexBuffer(indexData, indexCount, outId);
	}

	void DestroyVertexBuffer(RendererResourceID::IDType& id)
	{
		Renderer::DestroyVertexBuffer(id);
	}

	void DestroyIndexBuffer(RendererResourceID::IDType& id)
	{
		Renderer::DestroyIndexBuffer(id);
	}

	void BeginFrame()
	{
		static constexpr float clearColor[] = { 0.f, 0.15f, 0.275f, 1.f };
		static constexpr float clearDepth = 1.f;
		static constexpr unsigned char clearStencil = 0;

		Renderer::Clear(clearColor, clearDepth, clearStencil);
		Renderer::BeginRenderPass();
	}

	void EndFrame()
	{
	}

	void Draw(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceID::IDType vertexBufferId, const RendererResourceID::IDType indexBufferId)
	{
		Renderer::DrawIndexed(indexCount, singleVertexStrideBytes, vertexBufferId, indexBufferId);
	}

	bool SetObjectData(const ConstantBufferTypes::ObjectConstantBuffer& objectData)
	{
		return Renderer::SetObjectBufferData(objectData);
	}

	bool SetMaterialData(const ConstantBufferTypes::MaterialConstantBuffer& materialData)
	{
		return Renderer::SetMaterialBufferData(materialData);
	}

	void Present()
	{
		Renderer::Present();
	}
}
