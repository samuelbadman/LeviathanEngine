#include "LeviathanRenderer.h"
#include "Logging.h"
#include "Core.h"
#include "Renderer.h"

namespace LeviathanRenderer
{
	static void OnRuntimeWindowResized(int renderAreaWidth, int renderAreaHeight)
	{
		bool success = Renderer::ResizeWindowResources(renderAreaWidth, renderAreaHeight);

		if (!success)
		{
			LEVIATHAN_LOG("Failed to resize runtime window renderer resources.");
		}
	}

	bool Initialize()
	{
		static constexpr unsigned int bufferCount = 3;
		static constexpr bool vsync = false;

		// Register to runtime window callbacks.
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Register(&OnRuntimeWindowResized);

		// Initialize renderer api.
		void* platformHandle = LeviathanCore::Core::GetRuntimeWindowPlatformHandle();
		int width = 0;
		int height = 0;

		if (!LeviathanCore::Core::GetRuntimeWindowRenderAreaDimensions(width, height))
		{
			return false;
		}

		return Renderer::InitializeRendererApi(static_cast<unsigned int>(width), static_cast<unsigned int>(height), platformHandle, vsync, bufferCount);
	}

	bool Shutdown()
	{
		return Renderer::ShutdownRendererApi();
	}

	bool CreateVertexBuffer(const VertexTypes::Vertex1Pos* vertexData, unsigned int vertexCount, RendererResourceID::IDType& outId)
	{
		return Renderer::CreateVertexBuffer(vertexData, vertexCount, outId);
	}

	bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceID::IDType& outId)
	{
		return Renderer::CreateIndexBuffer(indexData, indexCount, outId);
	}

	void BeginFrame()
	{
		static constexpr float clearColor[] = { 1.f, 0.f, 0.f, 1.f };
		static constexpr float clearDepth = 1.f;
		static constexpr unsigned char clearStencil = 0;

		Renderer::Clear(clearColor, clearDepth, clearStencil);
		Renderer::BeginRenderPass();
	}

	void EndFrame()
	{
		Renderer::Present();
	}

	void Draw(const unsigned int indexCount, const RendererResourceID::IDType vertexBufferId, const RendererResourceID::IDType indexBufferId)
	{
		Renderer::DrawIndexed(indexCount, vertexBufferId, indexBufferId);
	}

	bool SetObjectData(const ConstantBufferTypes::ObjectConstantBuffer& objectData)
	{
		return Renderer::SetObjectBufferData(objectData);
	}

	bool SetMaterialData(const ConstantBufferTypes::MaterialConstantBuffer& materialData)
	{
		return Renderer::SetMaterialBufferData(materialData);
	}
}
