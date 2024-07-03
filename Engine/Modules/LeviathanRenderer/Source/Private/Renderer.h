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

	enum class TextureSamplerFilter : uint8_t;
	enum class TextureSamplerBorderMode : uint8_t;

	namespace Renderer
	{
		bool InitializeRendererApi(unsigned int width, unsigned int height, void* windowPlatformHandle, bool vsync, unsigned int bufferCount);
		bool ShutdownRendererApi();
		bool ResizeWindowResources(unsigned int width, unsigned int height);
		bool CreateVertexBuffer(const void* vertexData, unsigned int vertexCount, size_t singleVertexStrideBytes, RendererResourceID::IDType& outId);
		bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceID::IDType& outId);
		void DestroyVertexBuffer(RendererResourceID::IDType& resourceID);
		void DestroyIndexBuffer(RendererResourceID::IDType& resourceID);
		bool CreateTexture2D(uint32_t width, uint32_t height, const void* data, uint32_t rowPitchBytes, bool sRGB, RendererResourceID::IDType& outID);
		void DestroyTexture(RendererResourceID::IDType& resourceID);
		bool CreateSampler(TextureSamplerFilter filter, TextureSamplerBorderMode borderMode, const float* borderColor, RendererResourceID::IDType& outID);
		void DestroySampler(RendererResourceID::IDType& resourceID);

		// Render commands.
		void Clear(const float* clearColor, float clearDepth, unsigned char clearStencil);
		void BeginRenderPass();
		void Present();
		void DrawIndexed(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceID::IDType vertexBufferId, const RendererResourceID::IDType indexBufferId);
		bool UpdateObjectBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);
		bool UpdateSceneBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);

		// TODO: These are pipeline specific. Should be set for a specific render pass during begin render pass.
		void SetColorTexture2DResource(RendererResourceID::IDType texture2DId);
		void SetRoughnessTexture2DResource(RendererResourceID::IDType texture2DId);
		void SetMetallicTexture2DResource(RendererResourceID::IDType texture2DId);
		void SetNormalTexture2DResource(RendererResourceID::IDType texture2DId);
		void SetColorTextureSampler(RendererResourceID::IDType samplerId);
		void SetRoughnessTextureSampler(RendererResourceID::IDType samplerId);
		void SetMetallicTextureSampler(RendererResourceID::IDType samplerId);
		void SetNormalTextureSampler(RendererResourceID::IDType samplerId);

#ifdef LEVIATHAN_WITH_TOOLS
		bool ImGuiRendererInitialize();
		void ImGuiRendererShutdown();
		void ImGuiRendererNewFrame();
		void ImGuiRender();
#endif // LEVIATHAN_WITH_TOOLS.
	}
}