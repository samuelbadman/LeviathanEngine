#pragma once

namespace LeviathanRenderer
{
	namespace VertexTypes
	{
		struct Vertex1Pos;
	}

	namespace Renderer
	{
		[[nodiscard]] bool InitializeRendererApi(unsigned int width, unsigned int height, void* windowPlatformHandle, bool vsync, unsigned int bufferCount);
		[[nodiscard]] bool ShutdownRendererApi();
		[[nodiscard]] bool ResizeWindowResources(unsigned int width, unsigned int height);
		[[nodiscard]] bool CreateVertexBuffer(const VertexTypes::Vertex1Pos* vertexData, unsigned int vertexCount, int& outId);
		[[nodiscard]] bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, int& outId);

		void Clear(const float* clearColor, float clearDepth, unsigned char clearStencil);
		void BeginRenderPass();
		void Present();
		void DrawIndexed(const unsigned int indexCount, const int vertexBufferId, const int indexBufferId);
	}
}