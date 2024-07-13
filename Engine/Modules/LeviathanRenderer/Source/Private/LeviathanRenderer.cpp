#include "LeviathanRenderer.h"
#include "Logging.h"
#include "Core.h"
#include "Renderer.h"

namespace LeviathanRenderer
{
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

	void Render()
	{
		// Begin frame.

		// Set offscreen render target.
		static constexpr float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static constexpr float clearDepth = 1.0f;
		static constexpr uint8_t clearStencil = 0;

		Renderer::Clear(clearColor, clearDepth, clearStencil);
		Renderer::SetRenderTargets();

		// Enable depth testing and disable additive blending.

		// TODO: HDRI light pass.

		// Disable depth testing and enable additive blending.

		// Directional light pass.
		// Point light pass.
		// Spot light pass.

		// Note: Begin post processing.
		// Set screen render target.

		// Tone map and gamma correct pass.

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

	void BeginFrame()
	{
		//static constexpr float clearColor[] = { 0.0f, 0.15f, 0.275f, 1.0f };
		static constexpr float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static constexpr float clearDepth = 1.f;
		static constexpr unsigned char clearStencil = 0;

		Renderer::Clear(clearColor, clearDepth, clearStencil);
		Renderer::SetRenderTargets();
	}

	void EndFrame()
	{
	}

	void Draw(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceId::IdType vertexBufferId, const RendererResourceId::IdType indexBufferId)
	{
		Renderer::DrawIndexed(indexCount, singleVertexStrideBytes, vertexBufferId, indexBufferId);
	}

	void SetShaderResourceTables()
	{
		Renderer::SetShaderResourceTables();
	}

	bool UpdateObjectData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return Renderer::UpdateObjectBufferData(byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	bool UpdateDirectionalLightData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return Renderer::UpdateDirectionalLightBufferData(byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	bool UpdatePointLightData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return Renderer::UpdatePointLightBufferData(byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	bool UpdateSpotLightData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return Renderer::UpdateSpotLightBufferData(byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	void BeginDirectionalLightPass()
	{
		Renderer::SetDirectionalLightPipeline();
	}

	void BeginPointLightPass()
	{
		Renderer::SetPointLightPipeline();
	}

	void BeginSpotLightPass()
	{
		Renderer::SetSpotLightPipeline();
	}

	void SetColorTexture2D(RendererResourceId::IdType texture2DId)
	{
		Renderer::SetColorTexture2DResource(texture2DId);
	}

	void SetRoughnessTexture2D(RendererResourceId::IdType texture2DId)
	{
		Renderer::SetRoughnessTexture2DResource(texture2DId);
	}

	void SetMetallicTexture2D(RendererResourceId::IdType texture2DId)
	{
		Renderer::SetMetallicTexture2DResource(texture2DId);
	}

	void SetNormalTexture2D(RendererResourceId::IdType texture2DId)
	{
		Renderer::SetNormalTexture2DResource(texture2DId);
	}

	void SetColorTextureSampler(RendererResourceId::IdType samplerId)
	{
		Renderer::SetColorTextureSampler(samplerId);
	}

	void SetRoughnessTextureSampler(RendererResourceId::IdType samplerId)
	{
		Renderer::SetRoughnessTextureSampler(samplerId);
	}

	void SetMetallicTextureSampler(RendererResourceId::IdType samplerId)
	{
		Renderer::SetMetallicTextureSampler(samplerId);
	}

	void SetNormalTextureSampler(RendererResourceId::IdType samplerId)
	{
		Renderer::SetNormalTextureSampler(samplerId);
	}

	void Present()
	{
		Renderer::Present();
	}
}
