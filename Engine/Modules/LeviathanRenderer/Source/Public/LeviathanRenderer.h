#pragma once

#include "VertexTypes.h"
#include "ConstantBufferTypes.h"

namespace LeviathanRenderer
{
	static constexpr int InvalidId = -1;

	[[nodiscard]] bool Initialize();
	bool Shutdown();

	[[nodiscard]] bool CreateVertexBuffer(const VertexTypes::Vertex1Pos* vertexData, unsigned int vertexCount, int& outId);
	[[nodiscard]] bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, int& outId);

	void BeginFrame();
	void EndFrame();
	void Draw(const unsigned int indexCount, const int vertexBufferId, const int indexBufferId);
	bool SetMaterial(const ConstantBufferTypes::MaterialConstantBuffer& materialData);
}