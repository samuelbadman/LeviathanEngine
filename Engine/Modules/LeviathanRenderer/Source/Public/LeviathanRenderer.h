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
		TextureSamplerFilter filter = TextureSamplerFilter::FilterMax;
		TextureSamplerBorderMode borderMode = TextureSamplerBorderMode::BorderModeMax;
		// Pointer to a 4 element float array containing the color to use when border mode is set to solid color. float a[4] = {r, g, b, a}.
		const float* borderColor = nullptr;
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
	void DestroyTexture2D(RendererResourceID::IDType& id);
	[[nodiscard]] bool CreateTextureSampler(const TextureSamplerDescription& description, RendererResourceID::IDType& outID);
	void DestroyTextureSampler(RendererResourceID::IDType& id);

	void BeginFrame();
	void EndFrame();
	void Draw(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceID::IDType vertexBufferId, const RendererResourceID::IDType indexBufferId);
	bool UpdateObjectData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);
	bool UpdateSceneData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth);

	// TODO: Move into render pass.
	void SetColorTexture2D(RendererResourceID::IDType texture2DId);
	void SetRoughnessTexture2D(RendererResourceID::IDType texture2DId);
	void SetMetallicTexture2D(RendererResourceID::IDType texture2DId);
	void SetColorTextureSampler(RendererResourceID::IDType samplerId);
	void SetRoughnessTextureSampler(RendererResourceID::IDType samplerId);
	void SetMetallicTextureSampler(RendererResourceID::IDType samplerId);

	void Present();
}