#pragma once

#include "Callback.h"
#include "RendererResourceId.h"

namespace LeviathanRenderer
{
	namespace ConstantBufferTypes
	{
		struct DirectionalLightConstantBuffer;
		struct ObjectConstantBuffer;
		struct MaterialConstantBuffer;
	}

	enum class TextureSamplerFilter : uint8_t
	{
		Linear,
		Point,
		FilterMax
	};

	enum class TextureSamplerBorderMode : uint8_t
	{
		Wrap,
		Clamp,
		SolidColor,
		Mirror,
		BorderModeMax
	};

	struct Texture2DDescription
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		const void* Data = 0;
		uint32_t RowSizeBytes = 0;
		bool sRGB = false;
	};

	struct TextureSamplerDescription
	{
		TextureSamplerFilter Filter = TextureSamplerFilter::FilterMax;
		TextureSamplerBorderMode BorderMode = TextureSamplerBorderMode::BorderModeMax;
		// Pointer to a 4 element float array containing the color to use when border mode is set to solid color. float a[4] = {r, g, b, a}.
		const float* BorderColor = nullptr;
	};

	[[nodiscard]] bool Initialize();
	bool Shutdown();

#ifdef LEVIATHAN_WITH_TOOLS
	using RenderImGuiCallbackType = void(*)();
	LeviathanCore::Callback<RenderImGuiCallbackType>& GetRenderImGuiCallback();
#endif // LEVIATHAN_WITH_TOOLS.

	bool CreateVertexBuffer(const void* vertexData, unsigned int vertexCount, size_t singleVertexStrideBytes, RendererResourceId::IdType& outId);
	void DestroyVertexBuffer(RendererResourceId::IdType& id);
	bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceId::IdType& outId);
	void DestroyIndexBuffer(RendererResourceId::IdType& id);
	bool CreateTexture2D(const Texture2DDescription& description, RendererResourceId::IdType& outID);
	void DestroyTexture2D(RendererResourceId::IdType& id);
	bool CreateTextureSampler(const TextureSamplerDescription& description, RendererResourceId::IdType& outID);
	void DestroyTextureSampler(RendererResourceId::IdType& id);

	void BeginFrame();
	void EndFrame();
	void Draw(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceId::IdType vertexBufferId, const RendererResourceId::IdType indexBufferId);
	// Needs to be called when shader table data is updated or a new light pass is started.
	void SetShaderResourceTables();
	bool UpdateObjectData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);
	bool UpdateDirectionalLightData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);
	bool UpdatePointLightData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);
	bool UpdateSpotLightData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);
	void BeginDirectionalLightPass();
	void BeginPointLightPass();
	void BeginSpotLightPass();
	void Present();

	// Shader resource table data.
	void SetColorTexture2D(RendererResourceId::IdType texture2DId);
	void SetRoughnessTexture2D(RendererResourceId::IdType texture2DId);
	void SetMetallicTexture2D(RendererResourceId::IdType texture2DId);
	void SetNormalTexture2D(RendererResourceId::IdType texture2DId);
	void SetColorTextureSampler(RendererResourceId::IdType samplerId);
	void SetRoughnessTextureSampler(RendererResourceId::IdType samplerId);
	void SetMetallicTextureSampler(RendererResourceId::IdType samplerId);
	void SetNormalTextureSampler(RendererResourceId::IdType samplerId);
}