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
		struct DirectionalLightConstantBuffer;
		struct ObjectConstantBuffer;
	}

	enum class TextureSamplerFilter : uint8_t;
	enum class TextureSamplerBorderMode : uint8_t;

	namespace Renderer
	{
		bool InitializeRendererApi(unsigned int width, unsigned int height, void* windowPlatformHandle, bool vsync, unsigned int bufferCount);
		bool ShutdownRendererApi();
		bool ResizeWindowResources(unsigned int width, unsigned int height);
		bool CreateVertexBuffer(const void* vertexData, unsigned int vertexCount, size_t singleVertexStrideBytes, RendererResourceId::IdType& outId);
		bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceId::IdType& outId);
		void DestroyVertexBuffer(RendererResourceId::IdType& resourceID);
		void DestroyIndexBuffer(RendererResourceId::IdType& resourceID);
		bool CreateTexture2D(uint32_t width, uint32_t height, const void* data, uint32_t rowPitchBytes, bool sRGB, RendererResourceId::IdType& outID);
		void DestroyTexture(RendererResourceId::IdType& resourceID);
		bool CreateSampler(TextureSamplerFilter filter, TextureSamplerBorderMode borderMode, const float* borderColor, RendererResourceId::IdType& outID);
		void DestroySampler(RendererResourceId::IdType& resourceID);

		// Render commands.
		void Clear(const float* clearColor, float clearDepth, unsigned char clearStencil);
		void SetRenderTargets();
		void SetShaderResourceTables();
		void SetDirectionalLightPipeline();
		void Present();
		void DrawIndexed(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceId::IdType vertexBufferId, const RendererResourceId::IdType indexBufferId);
		bool UpdateObjectBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);
		bool UpdateDirectionalLightBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);

		// TODO: These are pipeline specific. Should be set for a specific render pass during begin render pass.
		void SetColorTexture2DResource(RendererResourceId::IdType texture2DId);
		void SetRoughnessTexture2DResource(RendererResourceId::IdType texture2DId);
		void SetMetallicTexture2DResource(RendererResourceId::IdType texture2DId);
		void SetNormalTexture2DResource(RendererResourceId::IdType texture2DId);
		void SetColorTextureSampler(RendererResourceId::IdType samplerId);
		void SetRoughnessTextureSampler(RendererResourceId::IdType samplerId);
		void SetMetallicTextureSampler(RendererResourceId::IdType samplerId);
		void SetNormalTextureSampler(RendererResourceId::IdType samplerId);

#ifdef LEVIATHAN_WITH_TOOLS
		bool ImGuiRendererInitialize();
		void ImGuiRendererShutdown();
		void ImGuiRendererNewFrame();
		void ImGuiRender();
#endif // LEVIATHAN_WITH_TOOLS.
	}
}