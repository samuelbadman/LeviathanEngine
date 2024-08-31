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
		Anisotropic,
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
		bool GenerateMipmaps = false;
		bool HDR = false;
	};

	struct TextureCubeDescription
	{
		uint32_t FaceWidth = 0;
		bool sRGB = false;
	};

	struct TextureSamplerDescription
	{
		TextureSamplerFilter Filter = TextureSamplerFilter::MAX;
		TextureSamplerBorderMode BorderMode = TextureSamplerBorderMode::MAX;
		// Pointer to a 4 element float array containing the unit rgba color to use when border mode is set to solid color. float a[4] = {r, g, b, a}.
		const float* BorderColor = nullptr;
		// Used when Filter is set to Anisotropic. Specified anisotropy level. Valid values in the range 1 - 16.
		uint32_t AnisotropyLevel = 1;
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
	bool CreateTextureCube(const TextureCubeDescription& description, RendererResourceId::IdType& outId);
	void DestroyTextureCube(RendererResourceId::IdType& id);
	void Render(const LeviathanRenderer::Camera& view, const LeviathanRenderer::Camera& skyboxView,
		RendererResourceId::IdType skyboxVertexBufferId, RendererResourceId::IdType skyboxIndexBufferId,
		const LeviathanRenderer::LightTypes::DirectionalLight* const pSceneDirectionalLights, const size_t numDirectionalLights,
		const LeviathanRenderer::LightTypes::PointLight* const pScenePointLights, const size_t numPointLights,
		const LeviathanRenderer::LightTypes::SpotLight* const pSceneSpotLights, const size_t numSpotLights, 
		const RendererResourceId::IdType skyboxTextureCubeResourceId, const RendererResourceId::IdType skyboxTextureCubeSamplerId,
		RendererResourceId::IdType colorTextureResourceId, RendererResourceId::IdType metallicTextureResourceId,
		RendererResourceId::IdType roughnessTextureResourceId, RendererResourceId::IdType normalTextureResourceId, RendererResourceId::IdType samplerResourceId,
		const LeviathanCore::MathTypes::Matrix4x4& objectTransformMatrix, const uint32_t objectIndexCount, RendererResourceId::IdType vertexBufferResourceId,
		RendererResourceId::IdType indexBufferResourceId);
	void Present();
}