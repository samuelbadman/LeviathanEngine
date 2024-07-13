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

	bool CreateVertexBuffer(const void* vertexData, unsigned int vertexCount, size_t singleVertexStrideBytes, RendererResourceId::IdType& outId)
	{
		return Renderer::CreateVertexBuffer(vertexData, vertexCount, singleVertexStrideBytes, outId);
	}

	bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceId::IdType& outId)
	{
		return Renderer::CreateIndexBuffer(indexData, indexCount, outId);
	}

	void DestroyVertexBuffer(RendererResourceId::IdType& id)
	{
		Renderer::DestroyVertexBuffer(id);
	}

	void DestroyIndexBuffer(RendererResourceId::IdType& id)
	{
		Renderer::DestroyIndexBuffer(id);
	}

	bool CreateTexture2D(const Texture2DDescription& description, RendererResourceId::IdType& outID)
	{
		return Renderer::CreateTexture2D(description.Width, description.Height, description.Data, description.RowSizeBytes, description.sRGB, outID);
	}

	void DestroyTexture2D(RendererResourceId::IdType& id)
	{
		Renderer::DestroyTexture(id);
	}

	bool CreateTextureSampler(const TextureSamplerDescription& description, RendererResourceId::IdType& outId)
	{
		return Renderer::CreateSampler(description.Filter, description.BorderMode, description.BorderColor, outId);
	}

	void DestroyTextureSampler(RendererResourceId::IdType& id)
	{
		Renderer::DestroySampler(id);
	}

	void BeginFrame()
	{
		//static constexpr float clearColor[] = { 0.0f, 0.15f, 0.275f, 1.0f };
		static constexpr float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static constexpr float clearDepth = 1.f;
		static constexpr unsigned char clearStencil = 0;

		Renderer::Clear(clearColor, clearDepth, clearStencil);
		Renderer::SetRenderTargets();
	}

	void EndFrame()
	{
	}

	void Draw(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceId::IdType vertexBufferId, const RendererResourceId::IdType indexBufferId)
	{
		Renderer::DrawIndexed(indexCount, singleVertexStrideBytes, vertexBufferId, indexBufferId);
	}

	void SetShaderResourceTables()
	{
		Renderer::SetShaderResourceTables();
	}

	bool UpdateObjectData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return Renderer::UpdateObjectBufferData(byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	bool UpdateDirectionalLightData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return Renderer::UpdateDirectionalLightBufferData(byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	bool UpdatePointLightData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return Renderer::UpdatePointLightBufferData(byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	bool UpdateSpotLightData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return Renderer::UpdateSpotLightBufferData(byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	void BeginDirectionalLightPass()
	{
		Renderer::SetDirectionalLightPipeline();
	}

	void BeginPointLightPass()
	{
		Renderer::SetPointLightPipeline();
	}

	void BeginSpotLightPass()
	{
		Renderer::SetSpotLightPipeline();
	}

	void SetColorTexture2D(RendererResourceId::IdType texture2DId)
	{
		Renderer::SetColorTexture2DResource(texture2DId);
	}

	void SetRoughnessTexture2D(RendererResourceId::IdType texture2DId)
	{
		Renderer::SetRoughnessTexture2DResource(texture2DId);
	}

	void SetMetallicTexture2D(RendererResourceId::IdType texture2DId)
	{
		Renderer::SetMetallicTexture2DResource(texture2DId);
	}

	void SetNormalTexture2D(RendererResourceId::IdType texture2DId)
	{
		Renderer::SetNormalTexture2DResource(texture2DId);
	}

	void SetColorTextureSampler(RendererResourceId::IdType samplerId)
	{
		Renderer::SetColorTextureSampler(samplerId);
	}

	void SetRoughnessTextureSampler(RendererResourceId::IdType samplerId)
	{
		Renderer::SetRoughnessTextureSampler(samplerId);
	}

	void SetMetallicTextureSampler(RendererResourceId::IdType samplerId)
	{
		Renderer::SetMetallicTextureSampler(samplerId);
	}

	void SetNormalTextureSampler(RendererResourceId::IdType samplerId)
	{
		Renderer::SetNormalTextureSampler(samplerId);
	}

	void Present()
	{
		Renderer::Present();
	}
}
