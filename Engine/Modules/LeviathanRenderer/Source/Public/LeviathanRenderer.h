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

	struct Texture2DDescription
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		const void* Data = 0;
		uint32_t RowSizeBytes = 0;
		bool sRGB = false;
	};

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
	[[nodiscard]] bool CreateTexture2D(const Texture2DDescription& description, RendererResourceID::IDType& outID);
	void DestroyTexture2D(RendererResourceID::IDType& outID);

	void BeginFrame();
	void EndFrame();
	void Draw(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceID::IDType vertexBufferId, const RendererResourceID::IDType indexBufferId);
	bool UpdateObjectData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);
	bool UpdateSceneData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);

	void SetColorTexture2D(RendererResourceID::IDType texture2DId);
	void SetRoughnessTexture2D(RendererResourceID::IDType texture2DId);
	void SetMetallicTexture2D(RendererResourceID::IDType texture2DId);

	void Present();
}