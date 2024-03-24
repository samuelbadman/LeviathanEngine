#include "TestTitle.h"
#include "LeviathanCore.h"
#include "LeviathanInputCore.h"
#include "LeviathanRenderer.h"
#include "MathTypes.h"
#include "MathLibrary.h"
#include "Camera.h"
#include "RendererResourceId.h"

namespace TestTitle
{
	static constexpr unsigned int gIndexCount = 6;
	static LeviathanRenderer::RendererResourceId::IdType gVertexBufferId = LeviathanRenderer::RendererResourceId::InvalidId;
	static LeviathanRenderer::RendererResourceId::IdType gIndexBufferId = LeviathanRenderer::RendererResourceId::InvalidId;

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

		// Quad 1 (dynamic).
		// Update material data.
		LeviathanRenderer::ConstantBufferTypes::MaterialConstantBuffer quadMaterialData = { {0.0f, 1.0f, 0.0f, 1.0f} };
		LeviathanRenderer::SetMaterialData(quadMaterialData);

		// Calculate world matrix.
		LeviathanCore::MathTypes::Matrix4x4 translationMatrix = LeviathanCore::MathTypes::Matrix4x4::Translation(LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, 0.0f));

		LeviathanCore::MathTypes::Matrix4x4 rotationMatrix = LeviathanCore::MathTypes::Matrix4x4::Rotation(
			LeviathanCore::MathTypes::Euler(0.0f, 0.0f, LeviathanCore::MathLibrary::DegreesToRadians(45.0f)));

		LeviathanCore::MathTypes::Matrix4x4 scalingMatrix = LeviathanCore::MathTypes::Matrix4x4::Scaling(LeviathanCore::MathTypes::Vector3(0.5f, 0.5f, 0.5f));

		LeviathanCore::MathTypes::Matrix4x4 worldMatrix = LeviathanCore::MathTypes::Matrix4x4::Identity();
		worldMatrix *= translationMatrix;
		worldMatrix *= rotationMatrix;
		worldMatrix *= scalingMatrix;

		// Calculate world view projection matrix.
		LeviathanCore::MathTypes::Matrix4x4 worldViewProjectionMatrix = {};
		worldViewProjectionMatrix = gSceneCamera.GetViewProjectionMatrix() * worldMatrix;

		// Update object data.
		LeviathanRenderer::ConstantBufferTypes::ObjectConstantBuffer quadObjectData = {};
		memcpy(quadObjectData.WorldViewProjection, worldViewProjectionMatrix.GetMatrix(), sizeof(float) * 16);
		LeviathanRenderer::SetObjectData(quadObjectData);

		// Draw.
		LeviathanRenderer::Draw(gIndexCount, gVertexBufferId, gIndexBufferId);

		// Quad 2 (dynamic).
		// Update material data.
		LeviathanRenderer::ConstantBufferTypes::MaterialConstantBuffer quadMaterialData2 = { {0.0f, 0.0f, 1.0f, 1.0f} };
		LeviathanRenderer::SetMaterialData(quadMaterialData2);

		// Calculate world matrix.
		LeviathanCore::MathTypes::Matrix4x4 translationMatrix2 = LeviathanCore::MathTypes::Matrix4x4::Translation(LeviathanCore::MathTypes::Vector3(2.0f, 0.0f, 0.0f));

		LeviathanCore::MathTypes::Matrix4x4 rotationMatrix2 = LeviathanCore::MathTypes::Matrix4x4::Rotation(
			LeviathanCore::MathTypes::Euler(0.0f, 0.0f, LeviathanCore::MathLibrary::DegreesToRadians(0.0f)));

		LeviathanCore::MathTypes::Matrix4x4 scalingMatrix2 = LeviathanCore::MathTypes::Matrix4x4::Scaling(LeviathanCore::MathTypes::Vector3(1.0f, 1.0f, 1.0f));

		LeviathanCore::MathTypes::Matrix4x4 worldMatrix2 = LeviathanCore::MathTypes::Matrix4x4::Identity();
		worldMatrix2 *= translationMatrix2;
		worldMatrix2 *= rotationMatrix2;
		worldMatrix2 *= scalingMatrix2;

		// Calculate world view projection matrix.
		LeviathanCore::MathTypes::Matrix4x4 worldViewProjectionMatrix2 = {};
		worldViewProjectionMatrix2 = gSceneCamera.GetViewProjectionMatrix() * worldMatrix2;

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
		LeviathanCore::Core::GetRuntimeWindowMouseInputCallback().Deregister(&OnRuntimeWindowMouseInput);

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
		LeviathanCore::Core::GetRuntimeWindowMouseInputCallback().Register(&OnRuntimeWindowMouseInput);

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
		gSceneCamera.SetPosition(LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, -5.0f));

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