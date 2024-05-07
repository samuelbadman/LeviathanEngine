#include "TestTitle.h"
#include "LeviathanCore.h"
#include "LeviathanInputCore.h"
#include "LeviathanRenderer.h"
#include "LeviathanAssets.h"

#include "AssetTypes.h"
#include "ModelImporter.h"
#include "MathTypes.h"
#include "MathLibrary.h"
#include "Camera.h"
#include "RendererResourceID.h"
#include "DataStructures.h"
#include "ConstantBufferTypes.h"
#include "VertexTypes.h"

#ifdef LEVIATHAN_WITH_TOOLS
#include "DemoTool.h"
#endif // LEVIATHAN_WITH_TOOLS.

namespace TestTitle
{
	struct Component
	{
		int Number = 0;
	};

	struct Transform
	{
		LeviathanCore::MathTypes::Vector3 Translation = {};
		LeviathanCore::MathTypes::Euler Rotation = {};
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

	static size_t gSingleVertexStrideBytes = 0;
	static unsigned int gIndexCount = 0;
	static LeviathanRenderer::RendererResourceID::IDType gVertexBufferId = LeviathanRenderer::RendererResourceID::InvalidID;
	static LeviathanRenderer::RendererResourceID::IDType gIndexBufferId = LeviathanRenderer::RendererResourceID::InvalidID;

	static LeviathanRenderer::Camera gSceneCamera = {};

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

	static void OnPostTick()
	{

	}

	static void OnInput([[maybe_unused]] LeviathanCore::InputKey key, [[maybe_unused]] bool isRepeatKey, [[maybe_unused]] float data)
	{
		static constexpr float cameraTranslationSpeed = 2.0f;

		static constexpr LeviathanCore::MathTypes::Vector3 forward(0.0f, 0.0f, 1.0f);
		static constexpr LeviathanCore::MathTypes::Vector3 right(1.0f, 0.0f, 0.0f);
		static constexpr LeviathanCore::MathTypes::Vector3 up(0.0f, 1.0f, 0.0f);

		static auto wasKeyPressed = [](float inData, bool inIsRepeatKey) { return ((inData == 1.0f) && (!inIsRepeatKey)); };
		static auto wasKeyReleased = [](float inData) { return (inData == 0.0f); };

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

	static void OnGameControllerInput([[maybe_unused]] LeviathanCore::InputKey key, [[maybe_unused]] bool isRepeatKey, [[maybe_unused]] float data, [[maybe_unused]] unsigned int gameControllerId)
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
		memcpy(sceneData.ViewMatrix, gSceneCamera.GetViewMatrix().Data(), sizeof(float) * 16);
		LeviathanRenderer::SetSceneData(sceneData);

		// Object 1 (dynamic).
		if (gIndexCount > 0)
		{
			// Update material data.
			LeviathanRenderer::ConstantBufferTypes::MaterialConstantBuffer materialData = 
			{
				.Color = {0.0f, 1.0f, 0.0f, 1.0f}
			};
			LeviathanRenderer::SetMaterialData(materialData);

			// Calculate world matrix.
			// TODO: Move scene updates into tick.
			[[maybe_unused]] const float deltaSeconds = LeviathanCore::Core::GetDeltaSeconds();

			static Transform objectTransform = {};
			objectTransform.Rotation.SetYawRadians(objectTransform.Rotation.GetYawRadians() + (0.75f * deltaSeconds));
			const LeviathanCore::MathTypes::Matrix4x4 worldMatrix = objectTransform.Matrix();

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
			LeviathanRenderer::SetObjectData(objectData);

			// Draw.
			LeviathanRenderer::Draw(gIndexCount, gSingleVertexStrideBytes, gVertexBufferId, gIndexBufferId);
		}

		// End frame.
		LeviathanRenderer::EndFrame();
	}

#ifdef LEVIATHAN_WITH_TOOLS
	static void OnRenderImGui()
	{
		//LeviathanTools::DemoTool::Render();
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
		LeviathanAssets::AssetTypes::Mesh model = LeviathanAssets::ModelImporter::GenerateCubePrimitive(0.5f);
		//std::vector<LeviathanAssets::AssetTypes::Mesh> model = {};
		//if (LeviathanAssets::ModelImporter::LoadModel("Model.fbx", model))
		{
			// Combine model meshes into a single mesh buffer.
			LeviathanAssets::AssetTypes::Mesh combinedModel = model;
			//LeviathanAssets::AssetTypes::Mesh combinedModel = LeviathanAssets::ModelImporter::CombineMeshes(model.data(), model.size());

			// Build render mesh.
			// Render mesh definition.
			std::vector<LeviathanRenderer::VertexTypes::VertexPosNorm> vertices = {};
			std::vector<unsigned int> indices = {};

			gSingleVertexStrideBytes = sizeof(LeviathanRenderer::VertexTypes::VertexPosNorm);
			gIndexCount = static_cast<unsigned int>(combinedModel.Indices.size());

			// Build render mesh.
			// For each vertex in the mesh.
			for (size_t i = 0; i < combinedModel.Positions.size(); ++i)
			{
				vertices.emplace_back(LeviathanRenderer::VertexTypes::VertexPosNorm
					{
						.Position = {combinedModel.Positions[i].GetX(), combinedModel.Positions[i].GetY(), combinedModel.Positions[i].GetZ()},
						.Normal = {combinedModel.Normals[i].GetX(), combinedModel.Normals[i].GetY(), combinedModel.Normals[i].GetZ()}
					});
			}

			// For each index in the mesh.
			for (size_t i = 0; i < combinedModel.Indices.size(); ++i)
			{
				// Offset indices into the vertex buffer as vertices from multiple meshes are being combin
				indices.push_back(combinedModel.Indices[i]);
			}

			// Create geometry buffers.
			if (!LeviathanRenderer::CreateVertexBuffer(vertices.data(), static_cast<unsigned int>(vertices.size()), sizeof(LeviathanRenderer::VertexTypes::VertexPosNorm), gVertexBufferId))
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

		// ECS module prototype code region.
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

		return true;
	}
}