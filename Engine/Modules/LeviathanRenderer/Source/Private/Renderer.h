#pragma once

#include "RendererResourceId.h"

namespace LeviathanRenderer
{
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
		bool CreateTexture2D(uint32_t width, uint32_t height, const void* data, uint32_t rowPitchBytes, bool sRGB, bool HDR, bool generateMips, RendererResourceId::IdType& outID);
		void DestroyTexture(RendererResourceId::IdType& resourceID);
		bool CreateSampler(TextureSamplerFilter filter, TextureSamplerBorderMode borderMode, const float* borderColor, const uint32_t anisotropy, RendererResourceId::IdType& outID);
		void DestroySampler(RendererResourceId::IdType& resourceID);
		bool CreateTextureCube(uint32_t faceWidth, const void* const * pFaceDatas, bool sRGB, RendererResourceId::IdType& outId);
		void DestroyTextureCube(RendererResourceId::IdType& resourceID);

		// Render commands.
		void SetEquirectangularToCubemapPipeline(RendererResourceId::IdType HDRTexture2DResourceId, RendererResourceId::IdType HDRTextureSamplerId);
		bool UpdateEquirectangularToCubemapBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);

		void SetSkyboxPipeline(RendererResourceId::IdType skyboxTextureCubeId, RendererResourceId::IdType skyboxTextureCubeSamplerId);
		bool UpdateSkyboxBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);

		void ClearScreenRenderTarget(const float* clearColor);
		void ClearSceneRenderTarget(const float* clearColor);
		void ClearRenderTarget(RendererResourceId::IdType renderTarget, const float* clearColor);
		void ClearDepthStencil(float clearDepth, unsigned char clearStencil);
		void SetScreenRenderTarget();
		void SetSceneRenderTarget();
		void SetRenderTarget(const RendererResourceId::IdType renderTargetId);
		void SetAmbientLightPipeline();
		void SetDirectionalLightPipeline();
		void SetPointLightPipeline();
		void SetSpotLightPipeline();
		void SetPostProcessPipeline();
		void Present();
		void DrawIndexed(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceId::IdType vertexBufferId, const RendererResourceId::IdType indexBufferId);
		bool UpdateObjectBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);
		bool UpdateDirectionalLightBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);
		bool UpdatePointLightBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);
		bool UpdateSpotLightBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);
		void UnbindShaderResources();
		void SetDepthStencilStateWriteDepthDepthFuncLessStencilDisabled();
		void SetDepthStencilStateWriteDepthDepthFuncLessEqualStencilDisabled();
		void SetDepthStencilStateDepthStencilDisabled();
		void SetDepthStencilStateNoWriteDepthDepthFuncEqualStencilDisabled();
		void SetDepthStencilStateNoWriteDepthDepthFuncLessStencilDisabled();
		void SetBlendStateAdditive();
		void SetBlendStateBlendDisabled();
		void SetViewport(uint32_t width, uint32_t height);

		void SetEnvironmentTextureCubeResource(RendererResourceId::IdType textureCubeId);

		void SetColorTexture2DResource(RendererResourceId::IdType texture2DId);
		void SetRoughnessTexture2DResource(RendererResourceId::IdType texture2DId);
		void SetMetallicTexture2DResource(RendererResourceId::IdType texture2DId);
		void SetNormalTexture2DResource(RendererResourceId::IdType texture2DId);

		void SetEnvironmentTextureSampler(RendererResourceId::IdType samplerId);
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