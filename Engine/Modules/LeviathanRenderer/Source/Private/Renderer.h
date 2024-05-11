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
		struct MaterialConstantBuffer;
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

		// Render commands.
		void Clear(const float* clearColor, float clearDepth, unsigned char clearStencil);
		void BeginRenderPass();
		void Present();
		void DrawIndexed(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceID::IDType vertexBufferId, const RendererResourceID::IDType indexBufferId);
		[[nodiscard]] bool UpdateSceneBufferData(const ConstantBufferTypes::SceneConstantBuffer& data);
		[[nodiscard]] bool UpdateObjectBufferData(const ConstantBufferTypes::ObjectConstantBuffer& data);
		[[nodiscard]] bool UpdateMaterialBufferData(const ConstantBufferTypes::MaterialConstantBuffer& data);

#ifdef LEVIATHAN_WITH_TOOLS
		bool ImGuiRendererInitialize();
		void ImGuiRendererShutdown();
		void ImGuiRendererNewFrame();
		void ImGuiRender();
#endif // LEVIATHAN_WITH_TOOLS.
	}
}