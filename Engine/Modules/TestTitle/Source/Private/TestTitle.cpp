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
#include "ConstantBufferTypes.h"
#include "VertexTypes.h"

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

	struct DirectionalLight
	{
		LeviathanCore::MathTypes::Vector3 Color{ 1.0f, 1.0f, 1.0f };
		float Brightness = 1.0f;
		// Direction in world space.
		LeviathanCore::MathTypes::Vector3 Direction{ -0.577350259f, -0.577350259f, 0.577350259f };
	};

	struct PointLight
	{
		LeviathanCore::MathTypes::Vector3 Color{ 1.0f, 1.0f, 1.0f };
		float Brightness = 1.0f;
		// Position in world space.
		LeviathanCore::MathTypes::Vector3 Position{ 0.0f, 0.0f, 0.0f };
	};

	struct SpotLight
	{
		LeviathanCore::MathTypes::Vector3 Color{ 1.0f, 1.0f, 1.0f };
		float Brightness = 1.0f;
		// Position in world space.
		LeviathanCore::MathTypes::Vector3 Position{ 0.0f, 0.0f, 0.0f };
		// Direction in world space.
		LeviathanCore::MathTypes::Vector3 Direction{ 0.0f, -1.0f, 0.0f };
		float InnerConeAngleRadians = LeviathanCore::MathLibrary::DegreesToRadians(0.0f);
		float OuterConeAngleRadians = LeviathanCore::MathLibrary::DegreesToRadians(17.5f);
	};

#ifdef LEVIATHAN_WITH_TOOLS
	static LeviathanTools::DemoTool gDemoTool = {};
	static LeviathanTools::PerfStatsDisplay gPerfStatsDisplay = {};
#endif // LEVIATHAN_WITH_TOOLS

	static size_t gSingleVertexStrideBytes = 0;
	static unsigned int gIndexCount = 0;
	static LeviathanRenderer::RendererResourceID::IDType gVertexBufferId = LeviathanRenderer::RendererResourceID::InvalidID;
	static LeviathanRenderer::RendererResourceID::IDType gIndexBufferId = LeviathanRenderer::RendererResourceID::InvalidID;

	static Transform gObjectTransform = {};

	static LeviathanRenderer::Camera gSceneCamera = {};

	static constexpr size_t gSceneDirectionalLightCount = 1;
	static DirectionalLight gSceneDirectionalLights[gSceneDirectionalLightCount] = {};

	static constexpr size_t gScenePointLightCount = 1;
	static PointLight gScenePointLights[gScenePointLightCount];

	static constexpr size_t gSceneSpotLightCount = 1;
	static SpotLight gSceneSpotLights[gSceneSpotLightCount];

	static LeviathanRenderer::RendererResourceID::IDType gColorTextureId = LeviathanRenderer::RendererResourceID::InvalidID;
	static LeviathanRenderer::RendererResourceID::IDType gRoughnessTextureId = LeviathanRenderer::RendererResourceID::InvalidID;
	static LeviathanRenderer::RendererResourceID::IDType gMetallicTextureId = LeviathanRenderer::RendererResourceID::InvalidID;
	static LeviathanRenderer::RendererResourceID::IDType gNormalTextureId = LeviathanRenderer::RendererResourceID::InvalidID;

	static LeviathanRenderer::RendererResourceID::IDType gLinearTextureSamplerId = LeviathanRenderer::RendererResourceID::InvalidID;
	static LeviathanRenderer::RendererResourceID::IDType gPointTextureSamplerId = LeviathanRenderer::RendererResourceID::InvalidID;

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
		//gObjectTransform.Rotation.SetYawRadians(gObjectTransform.Rotation.GetYawRadians() + (0.75f * deltaSeconds));
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
		// Begin frame.
		LeviathanRenderer::BeginFrame();

		// Update scene data.
		LeviathanRenderer::ConstantBufferTypes::SceneConstantBuffer sceneData = {};
		sceneData.DirectionalLightCount = 0;
		sceneData.PointLightCount = gScenePointLightCount;
		sceneData.SpotLightCount = 0;

		// For each directional light.
		for (size_t i = 0; i < gSceneDirectionalLightCount; ++i)
		{
			LeviathanCore::MathTypes::Vector3 directionalLightRadiance = gSceneDirectionalLights[i].Color * gSceneDirectionalLights[i].Brightness;
			LeviathanCore::MathTypes::Vector4 lightDirectionViewSpace4 = gSceneCamera.GetViewMatrix() * LeviathanCore::MathTypes::Vector4(gSceneDirectionalLights[i].Direction, 0.0f);
			LeviathanCore::MathTypes::Vector3 lightDirectionViewSpace{ lightDirectionViewSpace4.GetX(), lightDirectionViewSpace4.GetY(), lightDirectionViewSpace4.GetZ() };
			lightDirectionViewSpace.NormalizeSafe();

			LeviathanRenderer::ConstantBufferTypes::LightTypes::DirectionalLight& directionalLightData = *(sceneData.DirectionalLights + (i * 4));

			memcpy(&directionalLightData.Radiance, directionalLightRadiance.Data(), sizeof(float) * 3);
			memcpy(&directionalLightData.DirectionViewSpace, lightDirectionViewSpace.Data(), sizeof(float) * 3);
		}

		// For each point light.
		for (size_t i = 0; i < gScenePointLightCount; ++i)
		{
			LeviathanCore::MathTypes::Vector3 pointLightRadiance = gScenePointLights[i].Color * gScenePointLights[i].Brightness;
			LeviathanCore::MathTypes::Vector4 pointLightPositionViewSpace4 = gSceneCamera.GetViewMatrix() * LeviathanCore::MathTypes::Vector4{ gScenePointLights[i].Position, 1.0f };
			LeviathanCore::MathTypes::Vector3 pointLightPositionViewSpace{ pointLightPositionViewSpace4.GetX(), pointLightPositionViewSpace4.GetY(), pointLightPositionViewSpace4.GetZ() };

			LeviathanRenderer::ConstantBufferTypes::LightTypes::PointLight& pointLightData = *(sceneData.PointLights + (i * 4));

			memcpy(&pointLightData.Radiance, pointLightRadiance.Data(), sizeof(float) * 3);
			memcpy(&pointLightData.PositionViewSpace, pointLightPositionViewSpace.Data(), sizeof(float) * 3);
		}

		// For each spot light.
		for (size_t i = 0; i < gSceneSpotLightCount; ++i)
		{
			LeviathanCore::MathTypes::Vector3 spotLightRadiance = gSceneSpotLights[i].Color * gSceneSpotLights[i].Brightness;
			LeviathanCore::MathTypes::Vector4 spotLightPositionViewSpace4 = gSceneCamera.GetViewMatrix() * LeviathanCore::MathTypes::Vector4{ gSceneSpotLights[i].Position, 1.0f };
			LeviathanCore::MathTypes::Vector3 spotLightPositionViewSpace{ spotLightPositionViewSpace4.GetX(), spotLightPositionViewSpace4.GetY(), spotLightPositionViewSpace4.GetZ() };
			LeviathanCore::MathTypes::Vector4 spotLightDirectionViewSpace4 = gSceneCamera.GetViewMatrix() * LeviathanCore::MathTypes::Vector4{ gSceneSpotLights[i].Direction, 0.0f };
			LeviathanCore::MathTypes::Vector3 spotLightDirectionViewSpace{ spotLightDirectionViewSpace4.GetX(), spotLightDirectionViewSpace4.GetY(), spotLightDirectionViewSpace4.GetZ() };
			spotLightDirectionViewSpace.NormalizeSafe();

			LeviathanRenderer::ConstantBufferTypes::LightTypes::SpotLight& spotLightData = *(sceneData.SpotLights + (i * 4));

			memcpy(&spotLightData.Radiance, spotLightRadiance.Data(), sizeof(float) * 3);
			memcpy(&spotLightData.PositionViewSpace, spotLightPositionViewSpace.Data(), sizeof(float) * 3);
			memcpy(&spotLightData.DirectionViewSpace, spotLightDirectionViewSpace.Data(), sizeof(float) * 3);
			spotLightData.CosineInnerConeAngle = LeviathanCore::MathLibrary::Cos(gSceneSpotLights[i].InnerConeAngleRadians);
			spotLightData.CosineOuterConeAngle = LeviathanCore::MathLibrary::Cos(gSceneSpotLights[i].OuterConeAngleRadians);
		}

		LeviathanRenderer::UpdateSceneData(0, &sceneData, sizeof(LeviathanRenderer::ConstantBufferTypes::SceneConstantBuffer));

		// Object (dynamic).
		if (gIndexCount > 0)
		{
			// Update material data.
			LeviathanRenderer::SetColorTexture2D(gColorTextureId);
			LeviathanRenderer::SetRoughnessTexture2D(gRoughnessTextureId);
			LeviathanRenderer::SetMetallicTexture2D(gMetallicTextureId);
			LeviathanRenderer::SetNormalTexture2D(gNormalTextureId);

			LeviathanRenderer::SetColorTextureSampler(gLinearTextureSamplerId);
			LeviathanRenderer::SetRoughnessTextureSampler(gLinearTextureSamplerId);
			LeviathanRenderer::SetMetallicTextureSampler(gLinearTextureSamplerId);
			LeviathanRenderer::SetNormalTextureSampler(gLinearTextureSamplerId);

			// Calculate world matrix.
			const LeviathanCore::MathTypes::Matrix4x4 worldMatrix = gObjectTransform.Matrix();

			// Calculate world view matrix.
			const LeviathanCore::MathTypes::Matrix4x4 worldViewMatrix = gSceneCamera.GetViewMatrix() * worldMatrix;

			// Calculate normal matrix.
			const LeviathanCore::MathTypes::Matrix4x4 normalMatrix = LeviathanCore::MathTypes::Matrix4x4::Transpose(LeviathanCore::MathTypes::Matrix4x4::Inverse(worldViewMatrix));

			// Calculate world view projection matrix.
			const LeviathanCore::MathTypes::Matrix4x4 worldViewProjectionMatrix = gSceneCamera.GetViewProjectionMatrix() * worldMatrix;

			// Update object data.
			LeviathanRenderer::ConstantBufferTypes::ObjectConstantBuffer objectData = {};
			memcpy(objectData.WorldViewMatrix, worldViewMatrix.Data(), sizeof(float) * 16);
			memcpy(objectData.WorldViewProjectionMatrix, worldViewProjectionMatrix.Data(), sizeof(float) * 16);
			memcpy(objectData.NormalMatrix, worldViewMatrix.Data(), sizeof(float) * 16);
			LeviathanRenderer::UpdateObjectData(0, &objectData, sizeof(LeviathanRenderer::ConstantBufferTypes::ObjectConstantBuffer));

			// Draw.
			LeviathanRenderer::Draw(gIndexCount, gSingleVertexStrideBytes, gVertexBufferId, gIndexBufferId);
		}

		// End frame.
		LeviathanRenderer::EndFrame();
	}

#ifdef LEVIATHAN_WITH_TOOLS
	static void OnRenderImGui()
	{
		//gDemoTool.Render();
		gPerfStatsDisplay.Render(LeviathanCore::Core::GetPerfFPS(), LeviathanCore::Core::GetPerfMs());
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
		// Import model from disk.
		LeviathanAssets::AssetTypes::Mesh model = LeviathanAssets::ModelImporter::GeneratePlanePrimitive(0.5f);
		//LeviathanAssets::AssetTypes::Mesh model = LeviathanAssets::ModelImporter::GenerateCubePrimitive(0.5f);
		//LeviathanAssets::AssetTypes::Mesh model = LeviathanAssets::ModelImporter::GenerateSpherePrimitive(0.5f, 64, 64);
		//LeviathanAssets::AssetTypes::Mesh model = LeviathanAssets::ModelImporter::GenerateCylinderPrimitive(0.5f, 0.5f, 1.0f, 64, 64);
		//LeviathanAssets::AssetTypes::Mesh model = LeviathanAssets::ModelImporter::GenerateConePrimitive(0.5f, 1.0f, 64, 64);
		//std::vector<LeviathanAssets::AssetTypes::Mesh> model = {};
		//if (LeviathanAssets::ModelImporter::LoadModel("Cube.fbx", model))
		{
			// Combine model meshes into a single mesh buffer.
			LeviathanAssets::AssetTypes::Mesh combinedModel = model;
			//LeviathanAssets::AssetTypes::Mesh combinedModel = LeviathanAssets::ModelImporter::CombineMeshes(model.data(), model.size());

			// Build render mesh.
			// Render mesh definition.
			std::vector<LeviathanRenderer::VertexTypes::VertexPosNormUVTang> vertices = {};
			std::vector<unsigned int> indices = {};

			gSingleVertexStrideBytes = sizeof(LeviathanRenderer::VertexTypes::VertexPosNormUVTang);
			gIndexCount = static_cast<unsigned int>(combinedModel.Indices.size());

			// Build render mesh.
			// For each vertex in the mesh.
			for (size_t i = 0; i < combinedModel.Positions.size(); ++i)
			{
				vertices.emplace_back(LeviathanRenderer::VertexTypes::VertexPosNormUVTang
					{
						.Position = { combinedModel.Positions[i].GetX(), combinedModel.Positions[i].GetY(), combinedModel.Positions[i].GetZ() },
						.Normal = { combinedModel.Normals[i].GetX(), combinedModel.Normals[i].GetY(), combinedModel.Normals[i].GetZ() },
						.UV = { combinedModel.TextureCoordinates[i].GetX(), combinedModel.TextureCoordinates[i].GetY() },
						.Tangent = { combinedModel.Tangents[i].GetX(), combinedModel.Tangents[i].GetY(), combinedModel.Tangents[i].GetZ() }
					});
			}

			// For each index in the mesh.
			for (size_t i = 0; i < combinedModel.Indices.size(); ++i)
			{
				indices.push_back(combinedModel.Indices[i]);
			}

			// Create geometry buffers.
			if (!LeviathanRenderer::CreateVertexBuffer(vertices.data(), static_cast<unsigned int>(vertices.size()), sizeof(LeviathanRenderer::VertexTypes::VertexPosNormUVTang), gVertexBufferId))
			{
				return false;
			}

			if (!LeviathanRenderer::CreateIndexBuffer(indices.data(), static_cast<unsigned int>(indices.size()), gIndexBufferId))
			{
				return false;
			}
		}

		// Load quad geometry.
		//std::array<LeviathanRenderer::VertexTypes::Vertex1Pos, 4> quadVertices =
		//{
		//	LeviathanRenderer::VertexTypes::Vertex1Pos{ -0.5f, -0.5f, 0.0f }, // Bottom left.
		//	LeviathanRenderer::VertexTypes::Vertex1Pos{ -0.5f, 0.5f, 0.0f }, // Top left.
		//	LeviathanRenderer::VertexTypes::Vertex1Pos{ 0.5f, 0.5f, 0.0f }, // Top right.
		//	LeviathanRenderer::VertexTypes::Vertex1Pos{ 0.5f, -0.5f, 0.0f } // Bottom right.
		//};

		//std::array<unsigned int, 6> quadIndices =
		//{
		//	0, // Bottom left.
		//	1, // Top left.
		//	2, // Top right.
		//	0, // Bottom left.
		//	2, // Top right.
		//	3 // Bottom right.
		//};

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

		LeviathanRenderer::Texture2DDescription brickDiffuseTextureDesc = {};
		brickDiffuseTextureDesc.Width = brickDiffuseTexture.Width;
		brickDiffuseTextureDesc.Height = brickDiffuseTexture.Height;
		brickDiffuseTextureDesc.Data = brickDiffuseTexture.Data;
		brickDiffuseTextureDesc.RowSizeBytes = bytesPerPixel * brickDiffuseTexture.Width;
		brickDiffuseTextureDesc.sRGB = true;

		if (!LeviathanRenderer::CreateTexture2D(brickDiffuseTextureDesc, gColorTextureId))
		{
			LEVIATHAN_LOG("Failed to create brick diffuse texture resource.");
		}

		LeviathanRenderer::Texture2DDescription brickRoughnessTextureDesc = {};
		brickRoughnessTextureDesc.Width = brickRoughnessTexture.Width;
		brickRoughnessTextureDesc.Height = brickRoughnessTexture.Height;
		brickRoughnessTextureDesc.Data = brickRoughnessTexture.Data;
		brickRoughnessTextureDesc.RowSizeBytes = bytesPerPixel * brickRoughnessTexture.Width;
		brickRoughnessTextureDesc.sRGB = false;

		if (!LeviathanRenderer::CreateTexture2D(brickRoughnessTextureDesc, gRoughnessTextureId))
		{
			LEVIATHAN_LOG("Failed to create brick roughness texture resource.");
		}

		LeviathanRenderer::Texture2DDescription metallicTextureDesc = {};
		metallicTextureDesc.Width = 1;
		metallicTextureDesc.Height = 1;
		const uint32_t metallic = 0x010101;
		metallicTextureDesc.Data = static_cast<const void*>(&metallic);
		metallicTextureDesc.RowSizeBytes = bytesPerPixel * 1;
		metallicTextureDesc.sRGB = false;

		if (!LeviathanRenderer::CreateTexture2D(metallicTextureDesc, gMetallicTextureId))
		{
			LEVIATHAN_LOG("Failed to create metallic texture resource.");
		}

		LeviathanRenderer::Texture2DDescription brickNormalTextureDesc = {};
		brickNormalTextureDesc.Width = brickNormalTexture.Width;
		brickNormalTextureDesc.Height = brickNormalTexture.Height;
		brickNormalTextureDesc.Data = brickNormalTexture.Data;
		brickNormalTextureDesc.RowSizeBytes = bytesPerPixel * brickNormalTexture.Width;
		brickNormalTextureDesc.sRGB = false;

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

		// Define scene light.
		gSceneDirectionalLights[0].Color = LeviathanCore::MathTypes::Vector3{ 1.0f, 1.0f, 1.0f };
		gSceneDirectionalLights[0].Brightness = 1.0f;
		gSceneDirectionalLights[0].Direction = LeviathanCore::MathTypes::Vector3{ -1.0f, -1.0f, 1.0f }.AsNormalizedSafe();

		gScenePointLights[0].Color = LeviathanCore::MathTypes::Vector3{ 1.0f, 1.0f, 1.0f };
		gScenePointLights[0].Brightness = 1.0f;
		gScenePointLights[0].Position = LeviathanCore::MathTypes::Vector3{ 0.2f, 0.5f, -1.0f };

		gSceneSpotLights[0].Color = LeviathanCore::MathTypes::Vector3{ 1.0f, 1.0f, 1.0f };
		gSceneSpotLights[0].Brightness = 1.0f;
		gSceneSpotLights[0].Position = LeviathanCore::MathTypes::Vector3{ 0.0f, 0.0f, -1.5f };
		gSceneSpotLights[0].Direction = LeviathanCore::MathTypes::Vector3{ 0.0f, 0.0f, 1.0f }.AsNormalizedSafe();
		gSceneSpotLights[0].InnerConeAngleRadians = LeviathanCore::MathLibrary::DegreesToRadians(0.0f);
		gSceneSpotLights[0].OuterConeAngleRadians = LeviathanCore::MathLibrary::DegreesToRadians(17.5f);

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