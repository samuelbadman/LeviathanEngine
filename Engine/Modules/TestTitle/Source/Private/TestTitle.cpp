#include "TestTitle.h"
#include "LeviathanCore.h"
#include "LeviathanInputCore.h"
#include "LeviathanRenderer.h"
#include "LeviathanAssets.h"

#include "AssetTypes.h"
#include "ModelImporter.h"
#include "TextureImporter.h"
#include "MathTypes.h"
#include "MathLibrary.h"
#include "Camera.h"
#include "RendererResourceID.h"
#include "DataStructures.h"
#include "VertexTypes.h"
#include "LinearColor.h"
#include "LightTypes.h"

#ifdef LEVIATHAN_WITH_TOOLS
#include "DemoTool.h"
#include "PerfStatsDisplay.h"
#endif // LEVIATHAN_WITH_TOOLS.

namespace TestTitle
{
	struct Component
	{
		int Number = 0;
	};

	struct Transform
	{
		LeviathanCore::MathTypes::Vector3 Translation = { 0.0f, 0.0f, 0.0f };
		LeviathanCore::MathTypes::Euler Rotation = { 0.0f, 0.0f, 0.0f };
		LeviathanCore::MathTypes::Vector3 Scale = LeviathanCore::MathTypes::Vector3(1.0f, 1.0f, 1.0f);

		Transform() = default;
		Transform(const LeviathanCore::MathTypes::Vector3& translation, const LeviathanCore::MathTypes::Euler& rotation, const LeviathanCore::MathTypes::Vector3& scale)
			: Translation(translation), Rotation(rotation), Scale(scale)
		{}

		LeviathanCore::MathTypes::Matrix4x4 Matrix()
		{
			return LeviathanCore::MathTypes::Matrix4x4::Translation(Translation) *
				LeviathanCore::MathTypes::Matrix4x4::Rotation(Rotation) *
				LeviathanCore::MathTypes::Matrix4x4::Scaling(Scale);
		}
	};

#ifdef LEVIATHAN_WITH_TOOLS
	static LeviathanTools::DemoTool gDemoTool = {};
	static LeviathanTools::PerfStatsDisplay gPerfStatsDisplay = {};
#endif // LEVIATHAN_WITH_TOOLS

	static size_t gSingleVertexStrideBytes = 0;
	static unsigned int gIndexCount = 0;
	static LeviathanRenderer::RendererResourceId::IdType gVertexBufferId = LeviathanRenderer::RendererResourceId::InvalidId;
	static LeviathanRenderer::RendererResourceId::IdType gIndexBufferId = LeviathanRenderer::RendererResourceId::InvalidId;

	static Transform gObjectTransform = {};

	static LeviathanRenderer::Camera gSceneCamera = {};

	static std::vector<LeviathanRenderer::LightTypes::DirectionalLight> gSceneDirectionalLights = {};
	static std::vector<LeviathanRenderer::LightTypes::PointLight> gScenePointLights = {};
	static std::vector<LeviathanRenderer::LightTypes::SpotLight> gSceneSpotLights = {};

	static LeviathanRenderer::RendererResourceId::IdType gEnvironmentTextureId = LeviathanRenderer::RendererResourceId::InvalidId;

	static LeviathanRenderer::RendererResourceId::IdType gColorTextureId = LeviathanRenderer::RendererResourceId::InvalidId;
	static LeviathanRenderer::RendererResourceId::IdType gRoughnessTextureId = LeviathanRenderer::RendererResourceId::InvalidId;
	static LeviathanRenderer::RendererResourceId::IdType gMetallicTextureId = LeviathanRenderer::RendererResourceId::InvalidId;
	static LeviathanRenderer::RendererResourceId::IdType gDefaultNormalTextureId = LeviathanRenderer::RendererResourceId::InvalidId;
	static LeviathanRenderer::RendererResourceId::IdType gNormalTextureId = LeviathanRenderer::RendererResourceId::InvalidId;

	static LeviathanRenderer::RendererResourceId::IdType gLinearTextureSamplerId = LeviathanRenderer::RendererResourceId::InvalidId;
	static LeviathanRenderer::RendererResourceId::IdType gPointTextureSamplerId = LeviathanRenderer::RendererResourceId::InvalidId;

	static void OnRuntimeWindowResized(int renderAreaWidth, int renderAreaHeight)
	{
		gSceneCamera.UpdateProjectionMatrix(renderAreaWidth, renderAreaHeight);
		gSceneCamera.UpdateViewProjectionMatrix();
	}

	static void OnRuntimeWindowMouseInput(LeviathanCore::InputKey key, float data)
	{
		if (LeviathanInputCore::PlatformInput::IsKeyDown(LeviathanCore::InputKey::Keys::RightMouseButton))
		{
			static constexpr float cameraYawAbsoluteRotationRate = 0.075f;
			static constexpr float cameraPitchAbsoluteRotationRate = 0.00005f;

			switch (key.GetKey())
			{
			case LeviathanCore::InputKey::Keys::MouseXAxis:
			{
				// Add yaw rotation to the camera.
				const float yawDeltaDegrees = data * cameraYawAbsoluteRotationRate;
				gSceneCamera.AddYawRotation(LeviathanCore::MathLibrary::DegreesToRadians(yawDeltaDegrees));

				break;
			}

			case LeviathanCore::InputKey::Keys::MouseYAxis:
			{
				// Add pitch rotation to the camera.
				const float pitchDeltaDegrees = data * cameraPitchAbsoluteRotationRate;
				gSceneCamera.AddPitchRotation(LeviathanCore::MathLibrary::RadiansToDegrees(pitchDeltaDegrees));

				break;
			}
			}

			gSceneCamera.UpdateViewMatrix();
			gSceneCamera.UpdateViewProjectionMatrix();
		}
	}

	static void OnPreMainLoop()
	{

	}

	static void OnPostMainLoop()
	{

	}

	static void OnPreTick()
	{

	}

	static void OnFixedTick([[maybe_unused]] float fixedTimestep)
	{

	}

	static void OnTick([[maybe_unused]] float deltaSeconds)
	{
		if (LeviathanCore::Core::IsRuntimeWindowFocused())
		{
			// Poll input keys.
			LeviathanInputCore::PlatformInput::DispatchCallbackForKey(LeviathanCore::InputKey::Keys::RightMouseButton);
			LeviathanInputCore::PlatformInput::DispatchCallbackForKey(LeviathanCore::InputKey::Keys::F);

			if (LeviathanInputCore::PlatformInput::IsKeyDown(LeviathanCore::InputKey::Keys::RightMouseButton))
			{
				// Camera input keys.
				LeviathanInputCore::PlatformInput::DispatchCallbackForKey(LeviathanCore::InputKey::Keys::W);
				LeviathanInputCore::PlatformInput::DispatchCallbackForKey(LeviathanCore::InputKey::Keys::S);
				LeviathanInputCore::PlatformInput::DispatchCallbackForKey(LeviathanCore::InputKey::Keys::D);
				LeviathanInputCore::PlatformInput::DispatchCallbackForKey(LeviathanCore::InputKey::Keys::A);
				LeviathanInputCore::PlatformInput::DispatchCallbackForKey(LeviathanCore::InputKey::Keys::Q);
				LeviathanInputCore::PlatformInput::DispatchCallbackForKey(LeviathanCore::InputKey::Keys::E);

				// Lock mouse cursor to center of runtime window.
				int x = 0;
				int y = 0;
				LeviathanCore::Core::GetRuntimeWindowRenderAreaDimensions(x, y);
				LeviathanCore::Core::SetCursorPosInRuntimeWindow(x / 2, y / 2);
			}
		}

		// Update object transform.
		//gObjectTransform.Rotation.SetYawRadians(gObjectTransform.Rotation.YawRadians() + (0.75f * deltaSeconds));
		//gObjectTransform.Rotation.SetPitchRadians(gObjectTransform.Rotation.PitchRadians() + (0.75f * deltaSeconds));
	}

	static void OnPostTick()
	{

	}

	static void OnInput([[maybe_unused]] LeviathanCore::InputKey key, [[maybe_unused]] bool isRepeatKey, [[maybe_unused]] float data)
	{
		static constexpr float cameraTranslationSpeed = 2.0f;

		static constexpr LeviathanCore::MathTypes::Vector3 forward(0.0f, 0.0f, 1.0f);
		static constexpr LeviathanCore::MathTypes::Vector3 right(1.0f, 0.0f, 0.0f);
		static constexpr LeviathanCore::MathTypes::Vector3 up(0.0f, 1.0f, 0.0f);

		static constexpr auto wasKeyPressed = [](float inData, bool inIsRepeatKey) { return ((inData == 1.0f) && (!inIsRepeatKey)); };
		static constexpr auto wasKeyReleased = [](float inData) { return (inData == 0.0f); };

		const float deltaSeconds = LeviathanCore::Core::GetDeltaSeconds();

		switch (key.GetKey())
		{
		case LeviathanCore::InputKey::Keys::RightMouseButton:
		{
			if (wasKeyPressed(data, isRepeatKey))
			{
				LeviathanCore::Platform::ShowPlatformCursor(false);
			}
			else if (wasKeyReleased(data))
			{
				LeviathanCore::Platform::ShowPlatformCursor(true);
			}
			break;
		}

		case LeviathanCore::InputKey::Keys::F:
		{
			if (wasKeyPressed(data, isRepeatKey))
			{
				if (LeviathanCore::Core::IsRuntimeWindowFullscreen())
				{
					LeviathanCore::Core::RuntimeWindowExitFullscreen();
				}
				else
				{
					LeviathanCore::Core::RuntimeWindowEnterFullscreen();
				}
			}
			break;
		}

		case LeviathanCore::InputKey::Keys::W:
		{
			if (data == 1.0f)
			{
				gSceneCamera.SetPosition(gSceneCamera.GetPosition() + (gSceneCamera.GetForwardVector(forward) * deltaSeconds * cameraTranslationSpeed));
			}
			break;
		}

		case LeviathanCore::InputKey::Keys::S:
		{
			if (data == 1.0f)
			{
				gSceneCamera.SetPosition(gSceneCamera.GetPosition() - (gSceneCamera.GetForwardVector(forward) * deltaSeconds * cameraTranslationSpeed));
			}
			break;
		}

		case LeviathanCore::InputKey::Keys::D:
		{
			if (data == 1.0f)
			{
				gSceneCamera.SetPosition(gSceneCamera.GetPosition() + (gSceneCamera.GetRightVector(right) * deltaSeconds * cameraTranslationSpeed));
			}
			break;
		}

		case LeviathanCore::InputKey::Keys::A:
		{
			if (data == 1.0f)
			{
				gSceneCamera.SetPosition(gSceneCamera.GetPosition() - (gSceneCamera.GetRightVector(right) * deltaSeconds * cameraTranslationSpeed));
			}
			break;
		}

		case LeviathanCore::InputKey::Keys::E:
		{
			if (data == 1.0f)
			{
				// Use base up vector instead of camera up vector to always ensure vertical movement happens in the base up vector axis.
				gSceneCamera.SetPosition(gSceneCamera.GetPosition() + (up * deltaSeconds * cameraTranslationSpeed));
			}
			break;
		}

		case LeviathanCore::InputKey::Keys::Q:
		{
			if (data == 1.0f)
			{
				// Use base up vector instead of camera up vector to always ensure vertical movement happens in the base up vector axis.
				gSceneCamera.SetPosition(gSceneCamera.GetPosition() - (up * deltaSeconds * cameraTranslationSpeed));
			}
			break;
		}
		}

		gSceneCamera.UpdateViewMatrix();
		gSceneCamera.UpdateViewProjectionMatrix();
	}

	static void OnGameControllerInput([[maybe_unused]] LeviathanCore::InputKey key, [[maybe_unused]] bool isRepeatKey,
		[[maybe_unused]] float data, [[maybe_unused]] unsigned int gameControllerId)
	{

	}

	static void OnGameControllerConnected([[maybe_unused]] unsigned int gameControllerId)
	{

	}

	static void OnGameControllerDisconnected([[maybe_unused]] unsigned int gameControllerId)
	{

	}

	static void OnRender()
	{
		LeviathanRenderer::Render(gSceneCamera,
			gSceneDirectionalLights.data(), gSceneDirectionalLights.size(),
			gScenePointLights.data(), gScenePointLights.size(),
			gSceneSpotLights.data(), gSceneSpotLights.size(),
			gEnvironmentTextureId, gLinearTextureSamplerId,
			gColorTextureId, gMetallicTextureId, gRoughnessTextureId, gNormalTextureId, gLinearTextureSamplerId,
			gObjectTransform.Matrix(), gIndexCount, gVertexBufferId, gIndexBufferId);
	}

#ifdef LEVIATHAN_WITH_TOOLS
	static void OnRenderImGui()
	{
		//gDemoTool.Render();
		//gPerfStatsDisplay.Render(LeviathanCore::Core::GetPerfFPS(), LeviathanCore::Core::GetPerfMs());
	}
#endif // LEVIATHAN_WITH_TOOLS.

	static void OnPresent()
	{
		LeviathanRenderer::Present();
	}

	static void OnCleanup()
	{
		// Shutdown engine modules used by title.
		LeviathanAssets::Shutdown();
		LeviathanRenderer::Shutdown();
		LeviathanInputCore::Shutdown();

		// Deregister callbacks.
		LeviathanCore::Core::GetCleanupCallback().Deregister(&OnCleanup);
		LeviathanCore::Core::GetPreMainLoopCallback().Deregister(&OnPreMainLoop);
		LeviathanCore::Core::GetPostMainLoopCallback().Deregister(&OnPostMainLoop);
		LeviathanCore::Core::GetPreTickCallback().Deregister(&OnPreTick);
		LeviathanCore::Core::GetFixedTickCallback().Deregister(&OnFixedTick);
		LeviathanCore::Core::GetTickCallback().Deregister(&OnTick);
		LeviathanCore::Core::GetPostTickCallback().Deregister(&OnPostTick);
		LeviathanCore::Core::GetRenderCallback().Deregister(&OnRender);
		LeviathanCore::Core::GetPresentCallback().Deregister(&OnPresent);
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Deregister(&OnRuntimeWindowResized);
		LeviathanCore::Core::GetRuntimeWindowMouseInputCallback().Deregister(&OnRuntimeWindowMouseInput);

		LeviathanInputCore::PlatformInput::GetInputCallback().Deregister(&OnInput);
		LeviathanInputCore::PlatformInput::GetGameControllerInputCallback().Deregister(&OnGameControllerInput);
		LeviathanInputCore::PlatformInput::GetGameControllerConnectedCallback().Deregister(&OnGameControllerConnected);
		LeviathanInputCore::PlatformInput::GetGameControllerDisconnectedCallback().Deregister(&OnGameControllerDisconnected);

#ifdef LEVIATHAN_WITH_TOOLS
		LeviathanRenderer::GetRenderImGuiCallback().Deregister(&OnRenderImGui);
#endif // LEVIATHAN_WITH_TOOLS.
	}

	bool Initialize()
	{
		// Initialize engine modules for title.
		if (!LeviathanInputCore::Initialize())
		{
			return false;
		}

		if (!LeviathanRenderer::Initialize())
		{
			return false;
		}

		if (!LeviathanAssets::Initialize())
		{
			return false;
		}

		// Register callbacks.
		LeviathanCore::Core::GetCleanupCallback().Register(&OnCleanup);
		LeviathanCore::Core::GetPreMainLoopCallback().Register(&OnPreMainLoop);
		LeviathanCore::Core::GetPostMainLoopCallback().Register(&OnPostMainLoop);
		LeviathanCore::Core::GetPreTickCallback().Register(&OnPreTick);
		LeviathanCore::Core::GetFixedTickCallback().Register(&OnFixedTick);
		LeviathanCore::Core::GetTickCallback().Register(&OnTick);
		LeviathanCore::Core::GetPostTickCallback().Register(&OnPostTick);
		LeviathanCore::Core::GetRenderCallback().Register(&OnRender);
		LeviathanCore::Core::GetPresentCallback().Register(&OnPresent);
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Register(&OnRuntimeWindowResized);
		LeviathanCore::Core::GetRuntimeWindowMouseInputCallback().Register(&OnRuntimeWindowMouseInput);

		LeviathanInputCore::PlatformInput::GetInputCallback().Register(&OnInput);
		LeviathanInputCore::PlatformInput::GetGameControllerInputCallback().Register(&OnGameControllerInput);
		LeviathanInputCore::PlatformInput::GetGameControllerConnectedCallback().Register(&OnGameControllerConnected);
		LeviathanInputCore::PlatformInput::GetGameControllerDisconnectedCallback().Register(&OnGameControllerDisconnected);

#ifdef LEVIATHAN_WITH_TOOLS
		LeviathanRenderer::GetRenderImGuiCallback().Register(&OnRenderImGui);
#endif // LEVIATHAN_WITH_TOOLS.

		// Create scene.
		//LeviathanAssets::AssetTypes::Mesh model = LeviathanAssets::ModelImporter::CreatePlanePrimitive(0.5f);
		LeviathanAssets::AssetTypes::Mesh model = LeviathanAssets::ModelImporter::CreateCubePrimitive(0.5f);
		//LeviathanAssets::AssetTypes::Mesh model = LeviathanAssets::ModelImporter::CreateSpherePrimitive(0.5f, 64, 64);
		//LeviathanAssets::AssetTypes::Mesh model = LeviathanAssets::ModelImporter::CreateCylinderPrimitive(0.5f, 0.5f, 1.0f, 64, 64);
		//LeviathanAssets::AssetTypes::Mesh model = LeviathanAssets::ModelImporter::CreateConePrimitive(0.5f, 1.0f, 64, 64);
		// Import model from disk.
		//std::vector<LeviathanAssets::AssetTypes::Mesh> model = {};
		//if (LeviathanAssets::ModelImporter::LoadModel("Suzanne.fbx", model))
		{
			// Combine model meshes into a single mesh buffer.
			LeviathanAssets::AssetTypes::Mesh combinedModel = model;
			//LeviathanAssets::AssetTypes::Mesh combinedModel = LeviathanAssets::ModelImporter::CombineMeshes(model.data(), model.size());

			// Build render mesh.
			// Render mesh definition.
			std::vector<LeviathanRenderer::VertexTypes::VertexPos3Norm3UV2Tang3> vertices = {};
			std::vector<unsigned int> indices = {};

			gSingleVertexStrideBytes = sizeof(LeviathanRenderer::VertexTypes::VertexPos3Norm3UV2Tang3);
			gIndexCount = static_cast<unsigned int>(combinedModel.Indices.size());

			// Build render mesh.
			// For each vertex in the mesh.
			for (size_t i = 0; i < combinedModel.Positions.size(); ++i)
			{
				vertices.emplace_back(LeviathanRenderer::VertexTypes::VertexPos3Norm3UV2Tang3
					{
						.Position = { combinedModel.Positions[i].X(), combinedModel.Positions[i].Y(), combinedModel.Positions[i].Z() },
						.Normal = { combinedModel.Normals[i].X(), combinedModel.Normals[i].Y(), combinedModel.Normals[i].Z() },
						.UV = { combinedModel.TextureCoordinates[i].X(), combinedModel.TextureCoordinates[i].Y() },
						.Tangent = { combinedModel.Tangents[i].X(), combinedModel.Tangents[i].Y(), combinedModel.Tangents[i].Z() }
					});
			}

			// For each index in the mesh.
			for (size_t i = 0; i < combinedModel.Indices.size(); ++i)
			{
				indices.push_back(combinedModel.Indices[i]);
			}

			// Create geometry buffers.
			if (!LeviathanRenderer::CreateVertexBuffer(vertices.data(), static_cast<unsigned int>(vertices.size()), sizeof(LeviathanRenderer::VertexTypes::VertexPos3Norm3UV2Tang3), gVertexBufferId))
			{
				return false;
			}

			if (!LeviathanRenderer::CreateIndexBuffer(indices.data(), static_cast<unsigned int>(indices.size()), gIndexBufferId))
			{
				return false;
			}
		}

		// Import HDR environment texture and convert equirectangular to cubemap.
		// Import HRD environment texture. Imported image is in equirectangular format.
		LeviathanAssets::AssetTypes::Texture hdrEnvTexture = {};
		if (!LeviathanAssets::TextureImporter::LoadHDR("blocky_photo_studio_4k.hdr", hdrEnvTexture))
		{
			LEVIATHAN_LOG("Failed to load HDR environment texture from disk.");
		}

		// Render the equirectangular map projected onto each face of a unit cube into separate render targets (6 render targets).

		// Use each render target as the texture for a face of a cubemap texture.


		// Import textures.
		LeviathanAssets::AssetTypes::Texture brickDiffuseTexture = {};
		if (!LeviathanAssets::TextureImporter::LoadTexture("red_bricks_04_diff_1k.png", brickDiffuseTexture))
		{
			LEVIATHAN_LOG("Failed to load brick diffuse texture from disk.");
		}

		LeviathanAssets::AssetTypes::Texture brickRoughnessTexture = {};
		if (!LeviathanAssets::TextureImporter::LoadTexture("red_bricks_04_rough_1k.png", brickRoughnessTexture))
		{
			LEVIATHAN_LOG("Failed to load brick roughness texture from disk.");
		}

		LeviathanAssets::AssetTypes::Texture brickNormalTexture = {};
		if (!LeviathanAssets::TextureImporter::LoadTexture("red_bricks_04_nor_dx_1k.png", brickNormalTexture))
		{
			LEVIATHAN_LOG("Failed to load brick normal texture from disk.");
		}

		// Create texture resources.
		static constexpr uint32_t bytesPerPixel = 4;

		if (!LeviathanRenderer::CreateCubeTexture(gEnvironmentTextureId))
		{
			LEVIATHAN_LOG("Failed to create cube texture.");
		}

		LeviathanRenderer::Texture2DDescription brickDiffuseTextureDesc = {};
		if (brickDiffuseTexture.Data)
		{
			brickDiffuseTextureDesc.Width = brickDiffuseTexture.Width;
			brickDiffuseTextureDesc.Height = brickDiffuseTexture.Height;
			brickDiffuseTextureDesc.Data = brickDiffuseTexture.Data;
			brickDiffuseTextureDesc.RowSizeBytes = bytesPerPixel * brickDiffuseTexture.Width;
			brickDiffuseTextureDesc.sRGB = true;
			brickDiffuseTextureDesc.GenerateMipmaps = true;
		}
		else
		{
			brickDiffuseTextureDesc.Width = 1;
			brickDiffuseTextureDesc.Height = 1;
			const LeviathanRenderer::LinearColor fallbackColor(255, 0, 255, 255);
			brickDiffuseTextureDesc.Data = &fallbackColor;
			brickDiffuseTextureDesc.RowSizeBytes = bytesPerPixel * 1;
			brickDiffuseTextureDesc.sRGB = false;
			brickDiffuseTextureDesc.GenerateMipmaps = false;
		}
		if (!LeviathanRenderer::CreateTexture2D(brickDiffuseTextureDesc, gColorTextureId))
		{
			LEVIATHAN_LOG("Failed to create brick diffuse texture resource.");
		}

		LeviathanRenderer::Texture2DDescription brickRoughnessTextureDesc = {};
		if (brickRoughnessTexture.Data)
		{
			brickRoughnessTextureDesc.Width = brickRoughnessTexture.Width;
			brickRoughnessTextureDesc.Height = brickRoughnessTexture.Height;
			brickRoughnessTextureDesc.Data = brickRoughnessTexture.Data;
			brickRoughnessTextureDesc.RowSizeBytes = bytesPerPixel * brickRoughnessTexture.Width;
			brickRoughnessTextureDesc.sRGB = false;
			brickRoughnessTextureDesc.GenerateMipmaps = true;
		}
		else
		{
			brickRoughnessTextureDesc.Width = 1;
			brickRoughnessTextureDesc.Height = 1;
			const LeviathanRenderer::LinearColor fallbackRoughness(254, 0, 0, 0);
			brickRoughnessTextureDesc.Data = &fallbackRoughness;
			brickRoughnessTextureDesc.RowSizeBytes = bytesPerPixel * 1;
			brickRoughnessTextureDesc.sRGB = false;
			brickRoughnessTextureDesc.GenerateMipmaps = false;
		}
		if (!LeviathanRenderer::CreateTexture2D(brickRoughnessTextureDesc, gRoughnessTextureId))
		{
			LEVIATHAN_LOG("Failed to create brick roughness texture resource.");
		}

		LeviathanRenderer::Texture2DDescription metallicTextureDesc = {};
		metallicTextureDesc.Width = 1;
		metallicTextureDesc.Height = 1;
		const LeviathanRenderer::LinearColor metallicColor(1, 0, 0, 0);
		metallicTextureDesc.Data = static_cast<const void*>(&metallicColor);
		metallicTextureDesc.RowSizeBytes = bytesPerPixel * 1;
		metallicTextureDesc.sRGB = false;
		metallicTextureDesc.GenerateMipmaps = false;
		if (!LeviathanRenderer::CreateTexture2D(metallicTextureDesc, gMetallicTextureId))
		{
			LEVIATHAN_LOG("Failed to create metallic texture resource.");
		}

		LeviathanRenderer::Texture2DDescription defaultNormalTextureDesc = {};
		defaultNormalTextureDesc.Width = 1;
		defaultNormalTextureDesc.Height = 1;
		const LeviathanRenderer::LinearColor defaultNormalColor(128, 128, 255, 255);
		defaultNormalTextureDesc.Data = static_cast<const void*>(&defaultNormalColor);
		defaultNormalTextureDesc.RowSizeBytes = bytesPerPixel * 1;
		defaultNormalTextureDesc.sRGB = false;
		defaultNormalTextureDesc.GenerateMipmaps = false;
		if (!LeviathanRenderer::CreateTexture2D(defaultNormalTextureDesc, gDefaultNormalTextureId))
		{
			LEVIATHAN_LOG("Failed to create default normal texture resource.");
		}

		LeviathanRenderer::Texture2DDescription brickNormalTextureDesc = {};
		if (brickNormalTexture.Data)
		{
			brickNormalTextureDesc.Width = brickNormalTexture.Width;
			brickNormalTextureDesc.Height = brickNormalTexture.Height;
			brickNormalTextureDesc.Data = brickNormalTexture.Data;
			brickNormalTextureDesc.RowSizeBytes = bytesPerPixel * brickNormalTexture.Width;
			brickNormalTextureDesc.sRGB = false;
			brickNormalTextureDesc.GenerateMipmaps = true;
		}
		else
		{
			brickNormalTextureDesc = defaultNormalTextureDesc;
		}
		if (!LeviathanRenderer::CreateTexture2D(brickNormalTextureDesc, gNormalTextureId))
		{
			LEVIATHAN_LOG("Failed to create brick normal texture resource.");
		}

		// Create texture samplers.
		LeviathanRenderer::TextureSamplerDescription linearSamplerDesc = {};
		linearSamplerDesc.Filter = LeviathanRenderer::TextureSamplerFilter::Linear;
		linearSamplerDesc.BorderMode = LeviathanRenderer::TextureSamplerBorderMode::Wrap;
		if (!LeviathanRenderer::CreateTextureSampler(linearSamplerDesc, gLinearTextureSamplerId))
		{
			LEVIATHAN_LOG("Failed to create linear texture sampler.");
		}

		LeviathanRenderer::TextureSamplerDescription pointSamplerDesc = {};
		pointSamplerDesc.Filter = LeviathanRenderer::TextureSamplerFilter::Point;
		pointSamplerDesc.BorderMode = LeviathanRenderer::TextureSamplerBorderMode::Wrap;
		if (!LeviathanRenderer::CreateTextureSampler(pointSamplerDesc, gPointTextureSamplerId))
		{
			LEVIATHAN_LOG("Failed to create point texture sampler.");
		}

		// Define object transform.
		gObjectTransform = {};

		// Define scene camera.
		gSceneCamera.SetPosition(LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, -2.5f));
		gSceneCamera.UpdateViewMatrix();

		int windowWidth = 0;
		int windowHeight = 0;
		if (!LeviathanCore::Core::GetRuntimeWindowRenderAreaDimensions(windowWidth, windowHeight))
		{
			return false;
		}

		gSceneCamera.UpdateProjectionMatrix(windowWidth, windowHeight);
		gSceneCamera.UpdateViewProjectionMatrix();

		// Define scene lights.
		gSceneDirectionalLights =
		{
			//LeviathanRenderer::LightTypes::DirectionalLight
			//{
			//	.Color = LeviathanCore::MathTypes::Vector3{ 1.0f, 1.0f, 1.0f },
			//	.Brightness = 1.0f,
			//	.Direction = LeviathanCore::MathTypes::Vector3{ 0.0f, -1.0f, 1.0f }.AsNormalizedSafe()
			//},

			//LeviathanRenderer::LightTypes::DirectionalLight
			//{
			//	.Color = LeviathanCore::MathTypes::Vector3{ 1.0f, 1.0f, 1.0f },
			//	.Brightness = 1.0f,
			//	.Direction = LeviathanCore::MathTypes::Vector3{ 0.0f, -1.0f, -1.0f }.AsNormalizedSafe()
			//},

			//LeviathanRenderer::LightTypes::DirectionalLight
			//{
			//	.Color = LeviathanCore::MathTypes::Vector3{ 1.0f, 1.0f, 1.0f },
			//	.Brightness = 1.0f,
			//	.Direction = LeviathanCore::MathTypes::Vector3{ 1.0f, 1.0f, 0.0f }.AsNormalizedSafe()
			//},

			LeviathanRenderer::LightTypes::DirectionalLight
			{
				.Color = LeviathanCore::MathTypes::Vector3{ 1.0f, 1.0f, 1.0f },
				.Brightness = 1.0f,
				.Direction = LeviathanCore::MathTypes::Vector3{ -1.0f, -1.0f, 1.0f }.AsNormalizedSafe()
			}
		};

		gScenePointLights =
		{
			//LeviathanRenderer::LightTypes::PointLight
			//{
			//	.Color = LeviathanCore::MathTypes::Vector3{ 1.0f, 1.0f, 1.0f },
			//	.Brightness = 1.0f,
			//	.Position = LeviathanCore::MathTypes::Vector3{ 0.0f, 0.0f, -0.75f }
			//}
		};

		gSceneSpotLights =
		{
			//LeviathanRenderer::LightTypes::SpotLight
			//{
			//	.Color = LeviathanCore::MathTypes::Vector3{ 1.0f, 1.0f, 1.0f },
			//	.Brightness = 1.0f,
			//	.Position = LeviathanCore::MathTypes::Vector3{ 0.0f, 0.0f, -0.85f },
			//	.Direction = LeviathanCore::MathTypes::Vector3{ 0.0f, 0.0f, 1.0f }.AsNormalizedSafe(),
			//	.InnerConeAngleRadians = LeviathanCore::MathLibrary::DegreesToRadians(0.0f),
			//	.OuterConeAngleRadians = LeviathanCore::MathLibrary::DegreesToRadians(20.0f)
			//}
		};

		// ECS module prototype code region.
		/*
#pragma region
		// Prototype code for Leviathan Entities ECS module.
		[[maybe_unused]] size_t maxEntities = 512;

		// Create entities.
		const auto newEntity = []() {static size_t entity = 0; return entity++; };

		[[maybe_unused]] size_t entity0 = newEntity();
		[[maybe_unused]] size_t entity1 = newEntity();
		[[maybe_unused]] size_t entity2 = newEntity();

		// Create component type store.
		LeviathanCore::DataStructures::SparseArray<Component> components(maxEntities, 2);

		// Add components to entities.
		components.Add(entity0, { 5 });
		components.Add(entity2, { 10 });

		components.Remove(entity0);

		// Use entites and components.
		if (components.IsValidID(entity0))
		{
			LEVIATHAN_LOG("entity 0 has the component.");
		}
		else
		{
			LEVIATHAN_LOG("entity 0 does not have the component.");
		}

		if (components.IsValidID(entity1))
		{
			LEVIATHAN_LOG("entity 1 has the component.");
		}
		else
		{
			LEVIATHAN_LOG("entity 1 does not have the component.");
		}

		if (components.IsValidID(entity2))
		{
			LEVIATHAN_LOG("entity 2 has the component.");
		}
		else
		{
			LEVIATHAN_LOG("entity 2 does not have the component.");
		}

		const LeviathanCore::DataStructures::SparseArray<Component>::DenseValue* componentsStart = nullptr;
		size_t componentsCount = 0;
		components.GetValues(componentsStart, componentsCount);

		for (size_t i = 0; i < componentsCount; ++i)
		{
			LEVIATHAN_LOG("Component index: %d, data: %d", i, componentsStart[i].Value.Number);
		}

		[[maybe_unused]] const LeviathanCore::DataStructures::SparseArray<Component>::DenseValue& value = components.GetValue(entity2);
		LEVIATHAN_LOG("Retrieved value: data: %d", value.Value.Number);
#pragma endregion
*/
		return true;
	}
}