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
		[[nodiscard]] bool CreateVertexBuffer(const VertexTypes::Vertex1Pos* vertexData, unsigned int vertexCount, RendererResourceId::IdType& outId);
		[[nodiscard]] bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceId::IdType& outId);

		// Render commands.
		void Clear(const float* clearColor, float clearDepth, unsigned char clearStencil);
		void BeginRenderPass();
		void Present();
		void DrawIndexed(const unsigned int indexCount, const RendererResourceId::IdType vertexBufferId, const RendererResourceId::IdType indexBufferId);
		[[nodiscard]] bool SetObjectBufferData(const ConstantBufferTypes::ObjectConstantBuffer& data);
		[[nodiscard]] bool SetMaterialBufferData(const ConstantBufferTypes::MaterialConstantBuffer& data);
	}
}