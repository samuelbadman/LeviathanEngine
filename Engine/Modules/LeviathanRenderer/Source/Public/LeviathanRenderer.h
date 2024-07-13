#pragma once

#include "Callback.h"
#include "RendererResourceId.h"

namespace LeviathanCore
{
	namespace MathTypes
	{
		class Matrix4x4;
	}
}

namespace LeviathanRenderer
{
	namespace LightTypes
	{
		struct DirectionalLight;
		struct PointLight;
		struct SpotLight;
	}

	class Camera;

	enum class TextureSamplerFilter : uint8_t
	{
		Linear,
		Point,
		MAX
	};

	enum class TextureSamplerBorderMode : uint8_t
	{
		Wrap,
		Clamp,
		SolidColor,
		Mirror,
		MAX
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
		TextureSamplerFilter Filter = TextureSamplerFilter::MAX;
		TextureSamplerBorderMode BorderMode = TextureSamplerBorderMode::MAX;
		// Pointer to a 4 element float array containing the unit rgba color to use when border mode is set to solid color. float a[4] = {r, g, b, a}.
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

	void Render(const LeviathanRenderer::Camera& view, 
		const LeviathanRenderer::LightTypes::DirectionalLight* const pSceneDirectionalLights, const size_t numDirectionalLights,
		/*TODO: Temporary parameters. Make a material/object solution.*/ RendererResourceId::IdType colorTextureResourceId, RendererResourceId::IdType metallicTextureResourceId, 
		RendererResourceId::IdType roughnessTextureResourceId, RendererResourceId::IdType normalTextureResourceId, RendererResourceId::IdType samplerResourceId,
		const LeviathanCore::MathTypes::Matrix4x4& objectTransformMatrix, const uint32_t objectIndexCount, RendererResourceId::IdType vertexBufferResourceId, 
		RendererResourceId::IdType indexBufferResourceId);
	void Present();
}