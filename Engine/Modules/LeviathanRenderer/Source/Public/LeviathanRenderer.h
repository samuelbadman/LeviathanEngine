#pragma once

#include "Callback.h"
#include "RendererResourceId.h"

namespace LeviathanRenderer
{
	namespace ConstantBufferTypes
	{
		struct SceneConstantBuffer;
		struct ObjectConstantBuffer;
		struct MaterialConstantBuffer;
	}

	[[nodiscard]] bool Initialize();
	bool Shutdown();

#ifdef LEVIATHAN_WITH_TOOLS
	using RenderImGuiCallbackType = void(*)();
	LeviathanCore::Callback<RenderImGuiCallbackType>& GetRenderImGuiCallback();
#endif // LEVIATHAN_WITH_TOOLS.

	[[nodiscard]] bool CreateVertexBuffer(const void* vertexData, unsigned int vertexCount, size_t singleVertexStrideBytes, RendererResourceID::IDType& outId);
	[[nodiscard]] bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceID::IDType& outId);
	void DestroyVertexBuffer(RendererResourceID::IDType& id);
	void DestroyIndexBuffer(RendererResourceID::IDType& id);

	void BeginFrame();
	void EndFrame();
	void Draw(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceID::IDType vertexBufferId, const RendererResourceID::IDType indexBufferId);
	bool UpdateSceneData(const ConstantBufferTypes::SceneConstantBuffer& sceneData);
	bool UpdateObjectData(const ConstantBufferTypes::ObjectConstantBuffer& objectData);
	bool UpdateMaterialData(const ConstantBufferTypes::MaterialConstantBuffer& materialData);
	void Present();
}