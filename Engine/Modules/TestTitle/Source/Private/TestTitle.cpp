#include "TestTitle.h"
#include "LeviathanCore.h"
#include "LeviathanInputCore.h"
#include "LeviathanRenderer.h"
#include "MathTypes.h"
#include "MathLibrary.h"

namespace TestTitle
{
	static constexpr unsigned int gIndexCount = 6;
	static int gVertexBufferId = LeviathanRenderer::InvalidId;
	static int gIndexBufferid = LeviathanRenderer::InvalidId;

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

	}

	static void OnPostTick()
	{

	}

	static void OnInput([[maybe_unused]] LeviathanCore::InputKey key, [[maybe_unused]] bool isRepeatKey, [[maybe_unused]] float data)
	{

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
		LeviathanRenderer::BeginFrame();
		LeviathanRenderer::Draw(gIndexCount, gVertexBufferId, gIndexBufferid);
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

		LeviathanInputCore::PlatformInput::GetInputCallback().Register(&OnInput);
		LeviathanInputCore::PlatformInput::GetGameControllerInputCallback().Register(&OnGameControllerInput);
		LeviathanInputCore::PlatformInput::GetGameControllerConnectedCallback().Register(&OnGameControllerConnected);
		LeviathanInputCore::PlatformInput::GetGameControllerDisconnectedCallback().Register(&OnGameControllerDisconnected);

		// Create scene.
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

		if (!LeviathanRenderer::CreateIndexBuffer(quadIndices.data(), static_cast<unsigned int>(quadIndices.size()), gIndexBufferid))
		{
			return false;
		}

		LeviathanRenderer::ConstantBufferTypes::MaterialConstantBuffer quadMaterialData = { {0.0f, 1.0f, 0.0f, 1.0f} };

		if (!LeviathanRenderer::SetMaterialData(quadMaterialData))
		{
			return false;
		}

		LeviathanCore::MathTypes::Matrix4x4 translationMatrix = LeviathanCore::MathTypes::Matrix4x4::Translation(LeviathanCore::MathTypes::Vector3(-1.0f, 0.0f, 0.0f));

		LeviathanCore::MathTypes::Matrix4x4 rotationMatrix = LeviathanCore::MathTypes::Matrix4x4::Rotation(
			LeviathanCore::MathTypes::Euler(0.0f, 0.0f, LeviathanCore::MathLibrary::DegreesToRadians(45.0f)));

		LeviathanCore::MathTypes::Matrix4x4 scalingMatrix = LeviathanCore::MathTypes::Matrix4x4::Scaling(LeviathanCore::MathTypes::Vector3(1.0f, 0.25f, 1.0f));



		LeviathanCore::MathTypes::Matrix4x4 worldMatrix = LeviathanCore::MathTypes::Matrix4x4::Identity();
		worldMatrix = worldMatrix * scalingMatrix;
		worldMatrix = worldMatrix * rotationMatrix;
		worldMatrix = worldMatrix * translationMatrix;

		worldMatrix.TransposeInPlace();



		LeviathanCore::MathTypes::Matrix4x4 viewMatrix = LeviathanCore::MathTypes::Matrix4x4::View(LeviathanCore::MathTypes::Vector3(0.0, 0.0f, -1.0f),
			LeviathanCore::MathTypes::Euler(LeviathanCore::MathLibrary::DegreesToRadians(0.0f),
											LeviathanCore::MathLibrary::DegreesToRadians(0.0f),
											LeviathanCore::MathLibrary::DegreesToRadians(0.0f)));
		viewMatrix.TransposeInPlace();



		int width = 0;
		int height = 0;
		LeviathanCore::Core::GetRuntimeWindowRenderAreaDimensions(width, height);

		// TODO: Express fov as horizontal angle. Replace implementation of perspective projection with custom code instead of using DirectX math.
		LeviathanCore::MathTypes::Matrix4x4 projectionMatrix = LeviathanCore::MathTypes::Matrix4x4::PerspectiveProjection(LeviathanCore::MathLibrary::DegreesToRadians(90.0f),
			static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);

		//float orthoWidth = 5e-3f;
		//LeviathanCore::MathTypes::Matrix4x4 projectionMatrix = LeviathanCore::MathTypes::Matrix4x4::OrthographicProjection(static_cast<float>(width) * orthoWidth,
		//	static_cast<float>(height) * orthoWidth, 0.1f, 1000.0f);

		projectionMatrix.TransposeInPlace();



		LeviathanCore::MathTypes::Matrix4x4 worldViewProjectionMatrix = projectionMatrix * viewMatrix * worldMatrix;
		worldViewProjectionMatrix.TransposeInPlace();

		LeviathanRenderer::ConstantBufferTypes::ObjectConstantBuffer quadObjectData = {};
		memcpy(quadObjectData.WorldViewProjection, worldViewProjectionMatrix.GetMatrix(), sizeof(float) * 16);

		LeviathanRenderer::SetObjectData(quadObjectData);

		return true;
	}
}