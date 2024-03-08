#include "TestTitle.h"
#include "LeviathanCore.h"
#include "LeviathanInputCore.h"
#include "LeviathanRenderer.h"
#include "MathTypes.h"
#include "MathLibrary.h"
#include "Camera.h"

namespace TestTitle
{
	static constexpr unsigned int gIndexCount = 6;
	static int gVertexBufferId = LeviathanRenderer::InvalidId;
	static int gIndexBufferId = LeviathanRenderer::InvalidId;

	static LeviathanRenderer::Camera gSceneCamera = {};

	static void OnRuntimeWindowResized(int renderAreaWidth, int renderAreaHeight)
	{
		gSceneCamera.UpdateProjectionMatrix(renderAreaWidth, renderAreaHeight);
		gSceneCamera.UpdateViewProjectionMatrix();
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
		LeviathanInputCore::PlatformInput::DispatchCallbackForKey(LeviathanCore::InputKey::Keys::D);
		LeviathanInputCore::PlatformInput::DispatchCallbackForKey(LeviathanCore::InputKey::Keys::A);

		if (LeviathanInputCore::PlatformInput::IsKeyDown(LeviathanCore::InputKey::Keys::RightMouseButton))
		{
			LeviathanInputCore::PlatformInput::DispatchCallbackForKey(LeviathanCore::InputKey::Keys::MouseXAxis);
			LeviathanInputCore::PlatformInput::DispatchCallbackForKey(LeviathanCore::InputKey::Keys::MouseYAxis);
		}
	}

	static void OnPostTick()
	{

	}

	static void OnInput([[maybe_unused]] LeviathanCore::InputKey key, [[maybe_unused]] bool isRepeatKey, [[maybe_unused]] float data)
	{
		using namespace LeviathanCore::MathTypes;

		static constexpr float cameraTranslationSpeed = 1.0f;
		static constexpr float cameraYawRotationRate = 5.0f;
		static constexpr float cameraPitchRotationRate = 5.0f;

		static constexpr Vector3 right(1.0f, 0.0f, 0.0f);
		static constexpr Vector3 forward(0.0f, 0.0f, 1.0f);
		static constexpr Vector3 up(0.0f, 1.0f, 0.0f);

		const float deltaSeconds = LeviathanCore::Core::GetDeltaSeconds();

		if ((key.GetKey() == LeviathanCore::InputKey::Keys::D) && (data == 1.0f))
		{
			gSceneCamera.SetPosition(gSceneCamera.GetPosition() + (right * deltaSeconds * cameraTranslationSpeed));
		}
		else if ((key.GetKey() == LeviathanCore::InputKey::Keys::A) && (data == 1.0f))
		{
			gSceneCamera.SetPosition(gSceneCamera.GetPosition() - (right * deltaSeconds * cameraTranslationSpeed));
		}
		else if (key.GetKey() == LeviathanCore::InputKey::Keys::MouseXAxis)
		{
			// TODO: Debug why the camera makes large jumps in rotation sometimes.

			// Add yaw rotation to the camera.
			const float yawDelta = data * deltaSeconds * cameraYawRotationRate;
			gSceneCamera.AddYawRotation(yawDelta);
		}
		else if (key.GetKey() == LeviathanCore::InputKey::Keys::MouseYAxis)
		{
			// Add yaw rotation to the camera.
			const float ptichDelta = data * deltaSeconds * cameraPitchRotationRate;
			gSceneCamera.AddPitchRotation(ptichDelta);
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

		// Quad 1.
		// Update material data.
		LeviathanRenderer::ConstantBufferTypes::MaterialConstantBuffer quadMaterialData = { {0.0f, 1.0f, 0.0f, 1.0f} };
		LeviathanRenderer::SetMaterialData(quadMaterialData);

		// Calculate world matrix.
		LeviathanCore::MathTypes::Matrix4x4 translationMatrix = LeviathanCore::MathTypes::Matrix4x4::Translation(LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, 0.0f));

		LeviathanCore::MathTypes::Matrix4x4 rotationMatrix = LeviathanCore::MathTypes::Matrix4x4::Rotation(
			LeviathanCore::MathTypes::Euler(0.0f, 0.0f, LeviathanCore::MathLibrary::DegreesToRadians(0.0f)));

		LeviathanCore::MathTypes::Matrix4x4 scalingMatrix = LeviathanCore::MathTypes::Matrix4x4::Scaling(LeviathanCore::MathTypes::Vector3(1.0f, 1.0f, 1.0f));

		LeviathanCore::MathTypes::Matrix4x4 worldMatrix = LeviathanCore::MathTypes::Matrix4x4::Identity();
		worldMatrix = worldMatrix * scalingMatrix;
		worldMatrix = worldMatrix * rotationMatrix;
		worldMatrix = worldMatrix * translationMatrix;

		worldMatrix.TransposeInPlace();

		// Calculate world view projection matrix.
		LeviathanCore::MathTypes::Matrix4x4 worldViewProjectionMatrix = {};
		worldViewProjectionMatrix = gSceneCamera.GetViewProjectionMatrix() * worldMatrix;
		worldViewProjectionMatrix.TransposeInPlace();

		// Update object data.
		LeviathanRenderer::ConstantBufferTypes::ObjectConstantBuffer quadObjectData = {};
		memcpy(quadObjectData.WorldViewProjection, worldViewProjectionMatrix.GetMatrix(), sizeof(float) * 16);
		LeviathanRenderer::SetObjectData(quadObjectData);

		// Draw.
		LeviathanRenderer::Draw(gIndexCount, gVertexBufferId, gIndexBufferId);

		// Quad 2.
		// Update material data.
		LeviathanRenderer::ConstantBufferTypes::MaterialConstantBuffer quadMaterialData2 = { {0.0f, 0.0f, 1.0f, 1.0f} };
		LeviathanRenderer::SetMaterialData(quadMaterialData2);

		// Calculate world matrix.
		LeviathanCore::MathTypes::Matrix4x4 translationMatrix2 = LeviathanCore::MathTypes::Matrix4x4::Translation(LeviathanCore::MathTypes::Vector3(2.0f, 0.0f, 0.0f));

		LeviathanCore::MathTypes::Matrix4x4 rotationMatrix2 = LeviathanCore::MathTypes::Matrix4x4::Rotation(
			LeviathanCore::MathTypes::Euler(0.0f, 0.0f, LeviathanCore::MathLibrary::DegreesToRadians(0.0f)));

		LeviathanCore::MathTypes::Matrix4x4 scalingMatrix2 = LeviathanCore::MathTypes::Matrix4x4::Scaling(LeviathanCore::MathTypes::Vector3(1.0f, 1.0f, 1.0f));

		LeviathanCore::MathTypes::Matrix4x4 worldMatrix2 = LeviathanCore::MathTypes::Matrix4x4::Identity();
		worldMatrix2 = worldMatrix2 * scalingMatrix2;
		worldMatrix2 = worldMatrix2 * rotationMatrix2;
		worldMatrix2 = worldMatrix2 * translationMatrix2;

		worldMatrix2.TransposeInPlace();

		// Calculate world view projection matrix.
		LeviathanCore::MathTypes::Matrix4x4 worldViewProjectionMatrix2 = {};
		worldViewProjectionMatrix2 = gSceneCamera.GetViewProjectionMatrix() * worldMatrix2;
		worldViewProjectionMatrix2.TransposeInPlace();

		// Update object data.
		LeviathanRenderer::ConstantBufferTypes::ObjectConstantBuffer quadObjectData2 = {};
		memcpy(quadObjectData2.WorldViewProjection, worldViewProjectionMatrix2.GetMatrix(), sizeof(float) * 16);
		LeviathanRenderer::SetObjectData(quadObjectData2);

		// Draw.
		LeviathanRenderer::Draw(gIndexCount, gVertexBufferId, gIndexBufferId);

		// End frame.
		LeviathanRenderer::EndFrame();
	}

	static void OnCleanup()
	{
		// Shutdown engine modules used by title.
		LeviathanInputCore::Shutdown();
		LeviathanRenderer::Shutdown();

		// Deregister callbacks.
		LeviathanCore::Core::GetCleanupCallback().Deregister(&OnCleanup);
		LeviathanCore::Core::GetPreMainLoopCallback().Deregister(&OnPreMainLoop);
		LeviathanCore::Core::GetPostMainLoopCallback().Deregister(&OnPostMainLoop);
		LeviathanCore::Core::GetPreTickCallback().Deregister(&OnPreTick);
		LeviathanCore::Core::GetFixedTickCallback().Deregister(&OnFixedTick);
		LeviathanCore::Core::GetTickCallback().Deregister(&OnTick);
		LeviathanCore::Core::GetPostTickCallback().Deregister(&OnPostTick);
		LeviathanCore::Core::GetRenderCallback().Deregister(&OnRender);
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Deregister(&OnRuntimeWindowResized);

		LeviathanInputCore::PlatformInput::GetInputCallback().Deregister(&OnInput);
		LeviathanInputCore::PlatformInput::GetGameControllerInputCallback().Deregister(&OnGameControllerInput);
		LeviathanInputCore::PlatformInput::GetGameControllerConnectedCallback().Deregister(&OnGameControllerConnected);
		LeviathanInputCore::PlatformInput::GetGameControllerDisconnectedCallback().Deregister(&OnGameControllerDisconnected);
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

		// Register callbacks.
		LeviathanCore::Core::GetCleanupCallback().Register(&OnCleanup);
		LeviathanCore::Core::GetPreMainLoopCallback().Register(&OnPreMainLoop);
		LeviathanCore::Core::GetPostMainLoopCallback().Register(&OnPostMainLoop);
		LeviathanCore::Core::GetPreTickCallback().Register(&OnPreTick);
		LeviathanCore::Core::GetFixedTickCallback().Register(&OnFixedTick);
		LeviathanCore::Core::GetTickCallback().Register(&OnTick);
		LeviathanCore::Core::GetPostTickCallback().Register(&OnPostTick);
		LeviathanCore::Core::GetRenderCallback().Register(&OnRender);
		LeviathanCore::Core::GetRuntimeWindowResizedCallback().Register(&OnRuntimeWindowResized);

		LeviathanInputCore::PlatformInput::GetInputCallback().Register(&OnInput);
		LeviathanInputCore::PlatformInput::GetGameControllerInputCallback().Register(&OnGameControllerInput);
		LeviathanInputCore::PlatformInput::GetGameControllerConnectedCallback().Register(&OnGameControllerConnected);
		LeviathanInputCore::PlatformInput::GetGameControllerDisconnectedCallback().Register(&OnGameControllerDisconnected);

		// Create scene.
		// Load quad geometry.
		std::array<LeviathanRenderer::VertexTypes::Vertex1Pos, 4> quadVertices =
		{
			LeviathanRenderer::VertexTypes::Vertex1Pos{ -0.5f, -0.5f, 0.0f }, // Bottom left.
			LeviathanRenderer::VertexTypes::Vertex1Pos{ -0.5f, 0.5f, 0.0f }, // Top left.
			LeviathanRenderer::VertexTypes::Vertex1Pos{ 0.5f, 0.5f, 0.0f }, // Top right.
			LeviathanRenderer::VertexTypes::Vertex1Pos{ 0.5f, -0.5f, 0.0f } // Bottom right.
		};

		std::array<unsigned int, 6> quadIndices =
		{
			0, // Bottom left.
			1, // Top left.
			2, // Top right.
			0, // Bottom left.
			2, // Top right.
			3 // Bottom right.
		};

		if (!LeviathanRenderer::CreateVertexBuffer(quadVertices.data(), static_cast<unsigned int>(quadVertices.size()), gVertexBufferId))
		{
			return false;
		}

		if (!LeviathanRenderer::CreateIndexBuffer(quadIndices.data(), static_cast<unsigned int>(quadIndices.size()), gIndexBufferId))
		{
			return false;
		}

		// Define scene camera.
		gSceneCamera.SetPosition(LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, -2.0f));

		gSceneCamera.UpdateViewMatrix();

		int windowWidth = 0;
		int windowHeight = 0;
		if (!LeviathanCore::Core::GetRuntimeWindowRenderAreaDimensions(windowWidth, windowHeight))
		{
			return false;
		}

		gSceneCamera.UpdateProjectionMatrix(windowWidth, windowHeight);

		gSceneCamera.UpdateViewProjectionMatrix();

		return true;
	}
}