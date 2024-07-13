#include "LeviathanRenderer.h"
#include "Logging.h"
#include "Core.h"
#include "Renderer.h"
#include "ConstantBufferTypes.h"
#include "Camera.h"
#include "VertexTypes.h"

namespace LeviathanRenderer
{
	static uint32_t gScreenQuadIndexCount = 0;
	static RendererResourceId::IdType gScreenQuadVertexBufferResourceId = RendererResourceId::InvalidId;
	static RendererResourceId::IdType gScreenQuadIndexBufferResourceId = RendererResourceId::InvalidId;

#ifdef LEVIATHAN_WITH_TOOLS
	static LeviathanCore::Callback<RenderImGuiCallbackType> RenderImGuiCallback = {};
#endif // LEVIATHAN_WITH_TOOLS.

	static void OnRuntimeWindowResized(int renderAreaWidth, int renderAreaHeight)
	{
		bool success = Renderer::ResizeWindowResources(renderAreaWidth, renderAreaHeight);

		if (!success)
		{
			LEVIATHAN_LOG("Failed to resize runtime window renderer resources.");
		}
	}

#ifdef LEVIATHAN_WITH_TOOLS
	static void OnImGuiRendererNewFrame()
	{
		Renderer::ImGuiRendererNewFrame();
	}

	static void OnImGuiRender()
	{
		RenderImGuiCallback.Call();
		Renderer::ImGuiRender();
	}

	LeviathanCore::Callback<RenderImGuiCallbackType>& GetRenderImGuiCallback()
	{
		return RenderImGuiCallback;
	}
#endif // LEVIATHAN_WITH_TOOLS.

	static bool CreateScreenQuadGeometryResources(uint32_t& outIndexCount, RendererResourceId::IdType& outVertexBufferId, RendererResourceId::IdType& outIndexBufferId)
	{
		std::array<VertexTypes::VertexPos2UV2, 4> screenQuadVertices =
		{
			VertexTypes::VertexPos2UV2{ -1.0f, -1.0f, 0.0f, 0.0f }, // Bottom left.
			VertexTypes::VertexPos2UV2{ -1.0f, 1.0f, 0.0f, 1.0f }, // Top left.
			VertexTypes::VertexPos2UV2{ 1.0f, 1.0f, 1.0f, 1.0f }, // Top right.
			VertexTypes::VertexPos2UV2{ 1.0f, -1.0f,  1.0f, 0.0f } // Bottom right.
		};

		std::array<uint32_t, 6> screenQuadIndices =
		{
			0, // Bottom left.
			1, // Top left.
			2, // Top right.
			0, // Bottom left.
			2, // Top right.
			3 // Bottom right.
		};

		outIndexCount = static_cast<uint32_t>(screenQuadIndices.size());

		if (!CreateVertexBuffer(static_cast<const void*>(screenQuadVertices.data()), outIndexCount, sizeof(VertexTypes::VertexPos2UV2), outVertexBufferId))
		{
			return false;
		}

		if (!CreateIndexBuffer(screenQuadIndices.data(), static_cast<unsigned int>(screenQuadIndices.size()), outIndexBufferId))
		{
			return false;
		}

		return true;
	}

	bool Initialize()
	{
		static constexpr unsigned int bufferCount = 3;
		static constexpr bool vsync = false;

#ifdef LEVIATHAN_WITH_TOOLS
		// Clear render ImGui callback.
		RenderImGuiCallback.Clear();
#endif // LEVIATHAN_WITH_TOOLS.

		// Register to callbacks.
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Register(&OnRuntimeWindowResized);

#ifdef LEVIATHAN_WITH_TOOLS
		LeviathanCore::Core::GetImGuiRendererNewFrameCallback().Register(&OnImGuiRendererNewFrame);
		LeviathanCore::Core::GetImGuiRenderCallback().Register(&OnImGuiRender);
#endif // LEVIATHAN_WITH_TOOLS.

		// Initialize renderer api.
		void* platformHandle = LeviathanCore::Core::GetRuntimeWindowPlatformHandle();
		int width = 0;
		int height = 0;

		if (!LeviathanCore::Core::GetRuntimeWindowRenderAreaDimensions(width, height))
		{
			return false;
		}

		if (!Renderer::InitializeRendererApi(static_cast<unsigned int>(width), static_cast<unsigned int>(height), platformHandle, vsync, bufferCount))
		{
			return false;
		}

#ifdef LEVIATHAN_WITH_TOOLS
		if (!Renderer::ImGuiRendererInitialize())
		{
			return false;
		}
#endif // LEVIATHAN_WITH_TOOLS.

		// Create screen quad geometry for drawing post process effects.
		if (!CreateScreenQuadGeometryResources(gScreenQuadIndexCount, gScreenQuadVertexBufferResourceId, gScreenQuadIndexBufferResourceId))
		{
			return false;
		}

		return true;
	}

	bool Shutdown()
	{
		// Deregister from callbacks.
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Deregister(&OnRuntimeWindowResized);

#ifdef LEVIATHAN_WITH_TOOLS
		// Deregister from Leviathan tools callbacks.
		LeviathanCore::Core::GetImGuiRendererNewFrameCallback().Deregister(&OnImGuiRendererNewFrame);
		LeviathanCore::Core::GetImGuiRenderCallback().Deregister(&OnImGuiRender);

		// Shutdown Leviathan tools renderer.
		Renderer::ImGuiRendererShutdown();
#endif // LEVIATHAN_WITH_TOOLS.

		if (!Renderer::ShutdownRendererApi())
		{
			return false;
		}

		return true;
	}

	bool CreateVertexBuffer(const void* vertexData, unsigned int vertexCount, size_t singleVertexStrideBytes, RendererResourceId::IdType& outId)
	{
		return Renderer::CreateVertexBuffer(vertexData, vertexCount, singleVertexStrideBytes, outId);
	}

	bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceId::IdType& outId)
	{
		return Renderer::CreateIndexBuffer(indexData, indexCount, outId);
	}

	void DestroyVertexBuffer(RendererResourceId::IdType& id)
	{
		Renderer::DestroyVertexBuffer(id);
	}

	void DestroyIndexBuffer(RendererResourceId::IdType& id)
	{
		Renderer::DestroyIndexBuffer(id);
	}

	bool CreateTexture2D(const Texture2DDescription& description, RendererResourceId::IdType& outID)
	{
		return Renderer::CreateTexture2D(description.Width, description.Height, description.Data, description.RowSizeBytes, description.sRGB, outID);
	}

	void DestroyTexture2D(RendererResourceId::IdType& id)
	{
		Renderer::DestroyTexture(id);
	}

	bool CreateTextureSampler(const TextureSamplerDescription& description, RendererResourceId::IdType& outId)
	{
		return Renderer::CreateSampler(description.Filter, description.BorderMode, description.BorderColor, outId);
	}

	void DestroyTextureSampler(RendererResourceId::IdType& id)
	{
		Renderer::DestroySampler(id);
	}

	void Render([[maybe_unused]] const LeviathanRenderer::Camera& view,
		[[maybe_unused]] const LeviathanRenderer::LightTypes::DirectionalLight* const pSceneDirectionalLights, [[maybe_unused]] const size_t numDirectionalLights,
		/*TODO: Temporary parameters. Make a material/object solution.*/ [[maybe_unused]] RendererResourceId::IdType colorTextureResourceId, [[maybe_unused]] RendererResourceId::IdType metallicTextureResourceId,
		[[maybe_unused]] RendererResourceId::IdType roughnessTextureResourceId, [[maybe_unused]] RendererResourceId::IdType normalTextureResourceId, [[maybe_unused]] RendererResourceId::IdType samplerResourceId,
		[[maybe_unused]] const LeviathanCore::MathTypes::Matrix4x4& objectTransformMatrix, [[maybe_unused]] const uint32_t objectIndexCount, [[maybe_unused]] RendererResourceId::IdType objectVertexBufferResourceId,
		[[maybe_unused]] RendererResourceId::IdType objectIndexBufferResourceId)
	{
		// Begin frame.

		// Clear screen render target, scene render target and depth/stencil buffer.
		static constexpr float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		Renderer::ClearScreenRenderTarget(clearColor);
		Renderer::ClearSceneRenderTarget(clearColor);
		static constexpr float clearDepth = 1.0f;
		static constexpr uint8_t clearStencil = 0;
		Renderer::ClearDepthStencil(clearDepth, clearStencil);

		// Set offscreen render target.
		Renderer::SetSceneRenderTarget();

		// Enable depth testing and disable additive blending.
		Renderer::SetDepthTestEnabled();
		Renderer::SetBlendingDisabled();

		// TODO: HDRI light pass.

		// Disable depth testing and enable additive blending.
		Renderer::SetDepthTestDisabled();
		Renderer::SetBlendingAdditive();

		// Directional light pass.
		Renderer::SetDirectionalLightPipeline();
		for (size_t i = 0; i < numDirectionalLights; ++i)
		{
			LeviathanCore::MathTypes::Vector3 directionalLightRadiance = pSceneDirectionalLights[i].Color * pSceneDirectionalLights[i].Brightness;
			LeviathanCore::MathTypes::Vector4 lightDirectionViewSpace4 = view.GetViewMatrix() * LeviathanCore::MathTypes::Vector4(pSceneDirectionalLights[i].Direction, 0.0f);
			LeviathanCore::MathTypes::Vector3 lightDirectionViewSpace{ lightDirectionViewSpace4.X(), lightDirectionViewSpace4.Y(), lightDirectionViewSpace4.Z() };
			lightDirectionViewSpace.NormalizeSafe();
			LeviathanRenderer::ConstantBufferTypes::DirectionalLightConstantBuffer directionalLightData = {};
			memcpy(&directionalLightData.Radiance, directionalLightRadiance.Data(), sizeof(float) * 3);
			memcpy(&directionalLightData.LightDirectionViewSpace, lightDirectionViewSpace.Data(), sizeof(float) * 3);
			Renderer::UpdateDirectionalLightBufferData(0, static_cast<const void*>(&directionalLightData), sizeof(LeviathanRenderer::ConstantBufferTypes::DirectionalLightConstantBuffer));

			// TODO: For each object affected by light, daw.
			// Update shader table data.
			Renderer::SetColorTexture2DResource(colorTextureResourceId);
			Renderer::SetMetallicTexture2DResource(metallicTextureResourceId);
			Renderer::SetRoughnessTexture2DResource(roughnessTextureResourceId);
			Renderer::SetNormalTexture2DResource(normalTextureResourceId);

			Renderer::SetColorTextureSampler(samplerResourceId);
			Renderer::SetRoughnessTextureSampler(samplerResourceId);
			Renderer::SetMetallicTextureSampler(samplerResourceId);
			Renderer::SetNormalTextureSampler(samplerResourceId);

			Renderer::SetShaderResourceTables();

			const LeviathanCore::MathTypes::Matrix4x4 worldMatrix = objectTransformMatrix;
			const LeviathanCore::MathTypes::Matrix4x4 worldViewMatrix = view.GetViewMatrix() * worldMatrix;
			const LeviathanCore::MathTypes::Matrix4x4 normalMatrix = LeviathanCore::MathTypes::Matrix4x4::Transpose(LeviathanCore::MathTypes::Matrix4x4::Inverse(worldViewMatrix));
			const LeviathanCore::MathTypes::Matrix4x4 worldViewProjectionMatrix = view.GetViewProjectionMatrix() * worldMatrix;

			// Update object data.
			LeviathanRenderer::ConstantBufferTypes::ObjectConstantBuffer objectData = {};
			memcpy(objectData.WorldViewMatrix, worldViewMatrix.Data(), sizeof(float) * 16);
			memcpy(objectData.WorldViewProjectionMatrix, worldViewProjectionMatrix.Data(), sizeof(float) * 16);
			memcpy(objectData.NormalMatrix, worldViewMatrix.Data(), sizeof(float) * 16);
			Renderer::UpdateObjectBufferData(0, &objectData, sizeof(LeviathanRenderer::ConstantBufferTypes::ObjectConstantBuffer));

			// Draw.
			Renderer::DrawIndexed(objectIndexCount, sizeof(LeviathanRenderer::VertexTypes::VertexPos3Norm3UV2Tang3), objectVertexBufferResourceId, objectIndexBufferResourceId);
		}

		// Point light pass.
		// Spot light pass.

		// Disable blending.
		Renderer::SetBlendingDisabled();

		// Begin post processing.
		// Set screen render target.
		Renderer::SetScreenRenderTarget();

		// Post process pass.
		Renderer::SetPostProcessPipeline();
		Renderer::DrawIndexed(gScreenQuadIndexCount, sizeof(VertexTypes::VertexPos2UV2), gScreenQuadVertexBufferResourceId, gScreenQuadIndexBufferResourceId);

		// Unbind shader resources.
		Renderer::UnbindShaderResources();

		// End frame.



		//// Begin frame.
		//LeviathanRenderer::BeginFrame();

		//// Render directional lights.
		//LeviathanRenderer::BeginDirectionalLightPass();

		//// Update directional light data.
		//LeviathanRenderer::ConstantBufferTypes::DirectionalLightConstantBuffer directionalLightData = {};

		//LeviathanCore::MathTypes::Vector3 directionalLightRadiance = gSceneDirectionalLight.Color * gSceneDirectionalLight.Brightness;
		//LeviathanCore::MathTypes::Vector4 lightDirectionViewSpace4 = gSceneCamera.GetViewMatrix() * LeviathanCore::MathTypes::Vector4(gSceneDirectionalLight.Direction, 0.0f);
		//LeviathanCore::MathTypes::Vector3 lightDirectionViewSpace{ lightDirectionViewSpace4.X(), lightDirectionViewSpace4.Y(), lightDirectionViewSpace4.Z() };
		//lightDirectionViewSpace.NormalizeSafe();

		//memcpy(&directionalLightData.Radiance, directionalLightRadiance.Data(), sizeof(float) * 3);
		//memcpy(&directionalLightData.LightDirectionViewSpace, lightDirectionViewSpace.Data(), sizeof(float) * 3);

		//LeviathanRenderer::UpdateDirectionalLightData(0, static_cast<const void*>(&directionalLightData), sizeof(LeviathanRenderer::ConstantBufferTypes::DirectionalLightConstantBuffer));

		//// Draw objects.
		//DrawObject();

		//// Render point lights.
		//LeviathanRenderer::BeginPointLightPass();

		//// Update point light data.
		//LeviathanRenderer::ConstantBufferTypes::PointLightConstantBuffer pointLightData = {};

		//LeviathanCore::MathTypes::Vector3 pointLightRadiance = gScenePointLight.Color * gScenePointLight.Brightness;
		//LeviathanCore::MathTypes::Vector4 pointLightPositionViewSpace4 = gSceneCamera.GetViewMatrix() * LeviathanCore::MathTypes::Vector4{ gScenePointLight.Position, 1.0f };
		//LeviathanCore::MathTypes::Vector3 pointLightPositionViewSpace{ pointLightPositionViewSpace4.X(), pointLightPositionViewSpace4.Y(), pointLightPositionViewSpace4.Z() };

		//memcpy(&pointLightData.Radiance, pointLightRadiance.Data(), sizeof(float) * 3);
		//memcpy(&pointLightData.LightPositionViewSpace, pointLightPositionViewSpace.Data(), sizeof(float) * 3);

		//LeviathanRenderer::UpdatePointLightData(0, static_cast<const void*>(&pointLightData), sizeof(LeviathanRenderer::ConstantBufferTypes::PointLightConstantBuffer));

		//// Draw objects.
		//DrawObject();

		//// Render spot lights.
		//LeviathanRenderer::BeginSpotLightPass();

		//// Update spot light data.
		//LeviathanRenderer::ConstantBufferTypes::SpotLightConstantBuffer spotLightData = {};

		//LeviathanCore::MathTypes::Vector3 spotLightRadiance = gSceneSpotLight.Color * gSceneSpotLight.Brightness;
		//LeviathanCore::MathTypes::Vector4 spotLightPositionViewSpace4 = gSceneCamera.GetViewMatrix() * LeviathanCore::MathTypes::Vector4{ gSceneSpotLight.Position, 1.0f };
		//LeviathanCore::MathTypes::Vector3 spotLightPositionViewSpace{ spotLightPositionViewSpace4.X(), spotLightPositionViewSpace4.Y(), spotLightPositionViewSpace4.Z() };
		//LeviathanCore::MathTypes::Vector4 spotLightDirectionViewSpace4 = gSceneCamera.GetViewMatrix() * LeviathanCore::MathTypes::Vector4{ gSceneSpotLight.Direction, 0.0f };
		//LeviathanCore::MathTypes::Vector3 spotLightDirectionViewSpace{ spotLightDirectionViewSpace4.X(), spotLightDirectionViewSpace4.Y(), spotLightDirectionViewSpace4.Z() };
		//spotLightDirectionViewSpace.NormalizeSafe();

		//memcpy(&spotLightData.Radiance, spotLightRadiance.Data(), sizeof(float) * 3);
		//memcpy(&spotLightData.LightPositionViewSpace, spotLightPositionViewSpace.Data(), sizeof(float) * 3);
		//memcpy(&spotLightData.LightDirectionViewSpace, spotLightDirectionViewSpace.Data(), sizeof(float) * 3);
		//spotLightData.CosineInnerConeAngle = LeviathanCore::MathLibrary::Cos(gSceneSpotLight.InnerConeAngleRadians);
		//spotLightData.CosineOuterConeAngle = LeviathanCore::MathLibrary::Cos(gSceneSpotLight.OuterConeAngleRadians);

		//LeviathanRenderer::UpdateSpotLightData(0, static_cast<const void*>(&spotLightData), sizeof(LeviathanRenderer::ConstantBufferTypes::SpotLightConstantBuffer));

		//// Draw objects.
		//DrawObject();

		// End frame.
		//LeviathanRenderer::EndFrame();
	}

	void Present()
	{
		Renderer::Present();
	}
}
