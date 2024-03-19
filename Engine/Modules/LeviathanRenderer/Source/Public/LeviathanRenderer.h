#pragma once

#include "VertexTypes.h"
#include "ConstantBufferTypes.h"

#include "RendererResourceId.h"

namespace LeviathanRenderer
{
	static constexpr int InvalidId = -1;

	[[nodiscard]] bool Initialize();
	bool Shutdown();

	[[nodiscard]] bool CreateVertexBuffer(const VertexTypes::Vertex1Pos* vertexData, unsigned int vertexCount, RendererResourceId::IdType& outId);
	[[nodiscard]] bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceId::IdType& outId);

	void BeginFrame();
	void EndFrame();
	void Draw(const unsigned int indexCount, const RendererResourceId::IdType vertexBufferId, const RendererResourceId::IdType indexBufferId);
	bool SetObjectData(const ConstantBufferTypes::ObjectConstantBuffer& objectData);
	bool SetMaterialData(const ConstantBufferTypes::MaterialConstantBuffer& materialData);
}