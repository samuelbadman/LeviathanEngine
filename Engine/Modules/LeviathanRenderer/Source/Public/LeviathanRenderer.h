#pragma once

#include "Callback.h"
#include "VertexTypes.h"
#include "ConstantBufferTypes.h"

#include "RendererResourceId.h"

namespace LeviathanRenderer
{
	[[nodiscard]] bool Initialize();
	bool Shutdown();

#ifdef LEVIATHAN_WITH_TOOLS
	using RenderImGuiCallbackType = void(*)();
	LeviathanCore::Callback<RenderImGuiCallbackType>& GetRenderImGuiCallback();
#endif // LEVIATHAN_WITH_TOOLS.

	[[nodiscard]] bool CreateVertexBuffer(const VertexTypes::Vertex1Pos* vertexData, unsigned int vertexCount, RendererResourceID::IDType& outId);
	[[nodiscard]] bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceID::IDType& outId);
	void DestroyVertexBuffer(RendererResourceID::IDType& id);
	void DestroyIndexBuffer(RendererResourceID::IDType& id);

	void BeginFrame();
	void EndFrame();
	void Draw(const unsigned int indexCount, const RendererResourceID::IDType vertexBufferId, const RendererResourceID::IDType indexBufferId);
	bool SetObjectData(const ConstantBufferTypes::ObjectConstantBuffer& objectData);
	bool SetMaterialData(const ConstantBufferTypes::MaterialConstantBuffer& materialData);
	void Present();
}