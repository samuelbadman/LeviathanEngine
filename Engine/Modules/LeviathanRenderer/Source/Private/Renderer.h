#pragma once

#include "RendererResourceId.h"

namespace LeviathanRenderer
{
	namespace VertexTypes
	{
		struct VertexPos;
	}

	namespace ConstantBufferTypes
	{
		struct SceneConstantBuffer;
		struct ObjectConstantBuffer;
	}

	namespace Renderer
	{
		[[nodiscard]] bool InitializeRendererApi(unsigned int width, unsigned int height, void* windowPlatformHandle, bool vsync, unsigned int bufferCount);
		[[nodiscard]] bool ShutdownRendererApi();
		[[nodiscard]] bool ResizeWindowResources(unsigned int width, unsigned int height);
		[[nodiscard]] bool CreateVertexBuffer(const void* vertexData, unsigned int vertexCount, size_t singleVertexStrideBytes, RendererResourceID::IDType& outId);
		[[nodiscard]] bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceID::IDType& outId);
		void DestroyVertexBuffer(RendererResourceID::IDType& resourceID);
		void DestroyIndexBuffer(RendererResourceID::IDType& resourceID);
		[[nodiscard]] bool CreateTexture2D(uint32_t width, uint32_t height, const void* data, uint32_t rowPitchBytes, bool sRGB, RendererResourceID::IDType& outID);
		void DestroyTexture(RendererResourceID::IDType& resourceID);

		// Render commands.
		void Clear(const float* clearColor, float clearDepth, unsigned char clearStencil);
		void BeginRenderPass();
		void Present();
		void DrawIndexed(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceID::IDType vertexBufferId, const RendererResourceID::IDType indexBufferId);
		[[nodiscard]] bool UpdateObjectBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);
		[[nodiscard]] bool UpdateSceneBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);

		void SetColorTexture2DResource(RendererResourceID::IDType texture2DId);
		void SetRoughnessTexture2DResource(RendererResourceID::IDType texture2DId);
		void SetMetallicTexture2DResource(RendererResourceID::IDType texture2DId);

#ifdef LEVIATHAN_WITH_TOOLS
		bool ImGuiRendererInitialize();
		void ImGuiRendererShutdown();
		void ImGuiRendererNewFrame();
		void ImGuiRender();
#endif // LEVIATHAN_WITH_TOOLS.
	}
}