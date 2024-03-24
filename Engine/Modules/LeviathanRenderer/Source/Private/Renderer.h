#pragma once

#include "RendererResourceId.h"

namespace LeviathanRenderer
{
	namespace VertexTypes
	{
		struct Vertex1Pos;
	}

	namespace ConstantBufferTypes
	{
		struct ObjectConstantBuffer;
		struct MaterialConstantBuffer;
	}

	namespace Renderer
	{
		[[nodiscard]] bool InitializeRendererApi(unsigned int width, unsigned int height, void* windowPlatformHandle, bool vsync, unsigned int bufferCount);
		[[nodiscard]] bool ShutdownRendererApi();
		[[nodiscard]] bool ResizeWindowResources(unsigned int width, unsigned int height);
		[[nodiscard]] bool CreateVertexBuffer(const VertexTypes::Vertex1Pos* vertexData, unsigned int vertexCount, RendererResourceID::IDType& outId);
		[[nodiscard]] bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceID::IDType& outId);

		// Render commands.
		void Clear(const float* clearColor, float clearDepth, unsigned char clearStencil);
		void BeginRenderPass();
		void Present();
		void DrawIndexed(const unsigned int indexCount, const RendererResourceID::IDType vertexBufferId, const RendererResourceID::IDType indexBufferId);
		[[nodiscard]] bool SetObjectBufferData(const ConstantBufferTypes::ObjectConstantBuffer& data);
		[[nodiscard]] bool SetMaterialBufferData(const ConstantBufferTypes::MaterialConstantBuffer& data);
	}
}