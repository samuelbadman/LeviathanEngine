#include "Renderer.h"
#include "LeviathanAssert.h"
#include "VertexTypes.h"
#include "Logging.h"

namespace LeviathanRenderer
{
	namespace Renderer
	{
		// DXGI factory and adapter.
		static Microsoft::WRL::ComPtr<IDXGIFactory7> gDXGIFactory = {};
		static DXGI_ADAPTER_DESC1 gDXGIAdapterDesc = {};
		static Microsoft::WRL::ComPtr<IDXGIAdapter4> gDXGIAdapter = {};

		// Device, device context and swap chain.
		static Microsoft::WRL::ComPtr<ID3D11Device> gD3D11Device = {};
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext> gD3D11DeviceContext = {};
		static Microsoft::WRL::ComPtr<IDXGISwapChain> gSwapChain = {};
		static D3D_FEATURE_LEVEL gFeatureLevel = {};

		// Render target views of the swap chain back buffers.
		static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> gBackBufferRenderTargetView = {};

		// Depth/stencil target view of the depth/stencil buffer.
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilView> gDepthStencilView = {};

		// Texture resource to associate to the depth stencil view.
		static Microsoft::WRL::ComPtr<ID3D11Texture2D> gDepthStencilBuffer = {};

		// Define the functionality of the depth/stencil stage.
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> gDepthStencilState = {};

		// Define the functionality of the rasterizer stage.
		static Microsoft::WRL::ComPtr<ID3D11RasterizerState> gRasterizerState = {};
		static D3D11_VIEWPORT gViewport = {};

		// Shader compilation.
		static constexpr const char* SHADER_MODEL_5_VERTEX_SHADER = "vs_5_0";
		static constexpr const char* SHADER_MODEL_5_PIXEL_SHADER = "ps_5_0";
		//static constexpr const wchar_t* SHADER_MODEL_6_VERTEX_SHADER = L"vs_6_0";
		//static constexpr const wchar_t* SHADER_MODEL_6_PIXEL_SHADER = L"ps_6_0";

		//static Microsoft::WRL::ComPtr<IDxcUtils> DxcUtils = nullptr;
		//static Microsoft::WRL::ComPtr<IDxcCompiler> DxcCompiler = nullptr;

		// Define the renderer pipeline.
		static Microsoft::WRL::ComPtr<ID3D11InputLayout> gInputLayout = {};
		static std::vector<unsigned char> gVertexShaderBuffer = {};
		static Microsoft::WRL::ComPtr<ID3D11VertexShader> gVertexShader = {};
		static std::vector<unsigned char> gPixelShaderBuffer = {};
		static Microsoft::WRL::ComPtr<ID3D11PixelShader> gPixelShader = {};

		// Renderer state.
		static bool gVSync = false;

		// Scene resources.
		static Microsoft::WRL::ComPtr<ID3D11Buffer> gVertexBuffer = {};
		static Microsoft::WRL::ComPtr<ID3D11Buffer> gIndexBuffer = {};

		// Macro definitions.
#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
#define CHECK_HRESULT(hResult) LEVIATHAN_ASSERT(SUCCEEDED(hResult))
#else
#define CHECK_HRESULT(hResult)
#endif // LEVIATHAN_BUILD_CONFIG_DEBUG.

		// Shader source code.
		static const std::string gVertexShaderSourceCode = R"(
			struct VertexInput
			{
				float3 Position : POSITION;
			};

			struct VertexOutput
			{
				float4 Position : SV_POSITION;
			};

			VertexOutput main(VertexInput input)
			{
				VertexOutput output;
				output.Position = float4(input.Position, 1.0f);
				
				return output;
			}
		)";

		static const std::string gPixelShaderSourceCode = R"(
			struct PixelInput
			{
				float4 Position : SV_POSITION;
			};

			float4 main(PixelInput input) : SV_TARGET
			{
				return float4(1.0f, 1.0f, 1.0f, 1.0f);
			}
		)";

		static bool CompileHLSLStringFXC(const std::string_view string, const std::string_view entryPoint, const std::string_view name,
			const std::string_view target, std::vector<unsigned char>& outBuffer)
		{
			HRESULT hr = {};

			Microsoft::WRL::ComPtr<ID3DBlob> blob = {};
			Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = {};
			hr = D3DCompile(static_cast<const void*>(string.data()), string.size(), name.data(), nullptr, nullptr, entryPoint.data(),
				target.data(), D3DCOMPILE_ENABLE_STRICTNESS, 0, &blob, &errorBlob);

			if (FAILED(hr))
			{
				if (errorBlob)
				{
					LEVIATHAN_LOG("Failed to compile shader: %s, Error message: %s", name.data(), static_cast<const char*>(errorBlob->GetBufferPointer()));
					return false;
				}
			}

			// Copy result blob to output buffer.
			const SIZE_T compiledBlobSize = blob->GetBufferSize();
			outBuffer.resize(compiledBlobSize, 0);
			memcpy(outBuffer.data(), blob->GetBufferPointer(), compiledBlobSize);

			return true;
		}

		// Direct3D11 only supports shaders compiled with FXC which can only compile shader model 5 and below. Shader model 6 and above shaders must be compiled with
		// DXC and can only be used with Direct3D12.
		//static bool CompileHLSLStringDXC(IDxcUtils* dxcUtils, IDxcCompiler* dxcCompiler, const std::string_view string, const std::string_view entryPoint,
		//	const std::string_view name, const std::string_view target, std::vector<unsigned char>& outBuffer)
		//{
		//	HRESULT hr = {};

		//	// Create blob with source code.
		//	Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob = nullptr;
		//	hr = dxcUtils->CreateBlob(string.data(), static_cast<UINT32>(string.size()), CP_UTF8, &sourceBlob);
		//	CHECK_HRESULT(hr);

		//	// Compile blob containing source code.
		//	Microsoft::WRL::ComPtr<IDxcOperationResult> opResult = nullptr;
		//	hr = dxcCompiler->Compile(sourceBlob.Get(),
		//		std::wstring(name.begin(), name.end()).c_str(),
		//		std::wstring(entryPoint.begin(), entryPoint.end()).c_str(),
		//		std::wstring(target.begin(), target.end()).c_str(),
		//		nullptr,
		//		0,
		//		nullptr,
		//		0,
		//		nullptr,
		//		&opResult);
		//	CHECK_HRESULT(hr);

		//	// Check compilation result.
		//	HRESULT opResultStatus = 0;
		//	hr = opResult->GetStatus(&opResultStatus);
		//	CHECK_HRESULT(hr);

		//	if (FAILED(opResultStatus))
		//	{
		//		Microsoft::WRL::ComPtr<IDxcBlobEncoding> errorBlob = nullptr;
		//		hr = opResult->GetErrorBuffer(&errorBlob);
		//		CHECK_HRESULT(hr);

		//		if (errorBlob == nullptr)
		//		{
		//			return false;
		//		}

		//		LEVIATHAN_LOG("Failed to compile shader: %s, Error message: %s", name.data(), static_cast<const char*>(errorBlob->GetBufferPointer()));

		//		return false;
		//	}

		//	// Copy result blob to output buffer.
		//	Microsoft::WRL::ComPtr<IDxcBlob> compiledBlob = nullptr;
		//	hr = opResult->GetResult(&compiledBlob);
		//	CHECK_HRESULT(hr);

		//	const SIZE_T compiledBlobSize = compiledBlob->GetBufferSize();
		//	outBuffer.resize(compiledBlobSize, 0);
		//	memcpy(outBuffer.data(), compiledBlob->GetBufferPointer(), compiledBlobSize);

		//	return true;
		//}

		static void SetupViewport(unsigned int width, unsigned int height)
		{
			gViewport.Width = static_cast<float>(width);
			gViewport.Height = static_cast<float>(height);
			gViewport.TopLeftX = 0.f;
			gViewport.TopLeftY = 0.f;
			gViewport.MinDepth = 0.f;
			gViewport.MaxDepth = 1.f;
			gD3D11DeviceContext->RSSetViewports(1, &gViewport);
		}

		static void CreateBackBufferRenderTargetView()
		{
			HRESULT hr = {};

			Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer = nullptr;
			hr = gSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
			CHECK_HRESULT(hr);

			hr = gD3D11Device->CreateRenderTargetView(backBuffer.Get(), nullptr, &gBackBufferRenderTargetView);
			CHECK_HRESULT(hr);
		}

		static void CreateDepthStencilBufferAndView(unsigned int width, unsigned int height)
		{
			HRESULT hr = {};

			// Create the depth/stencil buffer.
			D3D11_TEXTURE2D_DESC depthStencilBufferDesc = {};
			depthStencilBufferDesc.ArraySize = 1;
			depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthStencilBufferDesc.CPUAccessFlags = 0;
			depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilBufferDesc.Width = width;
			depthStencilBufferDesc.Height = height;
			depthStencilBufferDesc.MipLevels = 1;
			depthStencilBufferDesc.SampleDesc.Count = 1;
			depthStencilBufferDesc.SampleDesc.Quality = 0;
			depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

			hr = gD3D11Device->CreateTexture2D(&depthStencilBufferDesc, nullptr, &gDepthStencilBuffer);
			CHECK_HRESULT(hr);

			// Create depth stencil view.
			hr = gD3D11Device->CreateDepthStencilView(gDepthStencilBuffer.Get(), nullptr, &gDepthStencilView);
			CHECK_HRESULT(hr);
		}

		void InitializeRendererApi(unsigned int width, unsigned int height, void* windowPlatformHandle, bool vsync, unsigned int bufferCount)
		{
			gVSync = vsync;

			HRESULT hr = {};

			// Create the DXGI factory.
			UINT createFactoryFlags = 0;
#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
			createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif //LEVIATHAN_BUILD_CONFIG_DEBUG.

			hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&gDXGIFactory));
			CHECK_HRESULT(hr);

			// Select adapter.
			hr = gDXGIFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&gDXGIAdapter));
			CHECK_HRESULT(hr);

			hr = gDXGIAdapter->GetDesc1(&gDXGIAdapterDesc);
			CHECK_HRESULT(hr);

			// Create device.
			UINT createDeviceFlags = 0;
#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
			createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif //LEVIATHAN_BUILD_CONFIG_DEBUG.

			std::array<D3D_FEATURE_LEVEL, 7> featureLevels =
			{
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3,
				D3D_FEATURE_LEVEL_9_2,
				D3D_FEATURE_LEVEL_9_1
			};

			hr = D3D11CreateDevice(gDXGIAdapter.Get(),
				D3D_DRIVER_TYPE_UNKNOWN,
				NULL,
				createDeviceFlags,
				featureLevels.data(), static_cast<unsigned int>(featureLevels.size()),
				D3D11_SDK_VERSION,
				&gD3D11Device, &gFeatureLevel, &gD3D11DeviceContext);
			CHECK_HRESULT(hr);

			// Create the swap chain.
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.Width = width;
			swapChainDesc.Height = height;
			swapChainDesc.BufferCount = bufferCount;
			swapChainDesc.Stereo = false;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			swapChainDesc.Flags = 0;
			// Flip model swap chains (DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL and DXGI_SWAP_EFFECT_FLIP_DISCARD) only support the following Formats: DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R10G10B10A2_UNORM.
			swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

			Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1 = {};
			hr = gDXGIFactory->CreateSwapChainForHwnd(gD3D11Device.Get(), static_cast<HWND>(windowPlatformHandle), &swapChainDesc, nullptr, nullptr, &swapChain1);
			CHECK_HRESULT(hr);

			// Disable alt + enter fullscreen shortcut. Must be called after creating the swap chain.
			hr = gDXGIFactory->MakeWindowAssociation(static_cast<HWND>(windowPlatformHandle), DXGI_MWA_NO_ALT_ENTER);
			CHECK_HRESULT(hr);

			// Convert swap chain 1 interface to swap chain 4 interface.
			hr = swapChain1.As(&gSwapChain);
			CHECK_HRESULT(hr);

			// Create render target view.
			CreateBackBufferRenderTargetView();

			// Create the depth/stencil buffer and view.
			CreateDepthStencilBufferAndView(width, height);

			// Create depth stencil state.
			D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = {};
			depthStencilStateDesc.DepthEnable = TRUE;
			depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
			depthStencilStateDesc.StencilEnable = FALSE;

			hr = gD3D11Device->CreateDepthStencilState(&depthStencilStateDesc, &gDepthStencilState);
			CHECK_HRESULT(hr);

			// Create rasterizer state.
			D3D11_RASTERIZER_DESC rasterizerStateDesc = {};
			rasterizerStateDesc.AntialiasedLineEnable = FALSE;
			rasterizerStateDesc.CullMode = D3D11_CULL_NONE;
			rasterizerStateDesc.DepthBias = 0;
			rasterizerStateDesc.DepthBiasClamp = 0.f;
			rasterizerStateDesc.DepthClipEnable = TRUE;
			rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerStateDesc.FrontCounterClockwise = FALSE;
			rasterizerStateDesc.MultisampleEnable = FALSE;
			rasterizerStateDesc.ScissorEnable = FALSE;
			rasterizerStateDesc.SlopeScaledDepthBias = 0.f;

			hr = gD3D11Device->CreateRasterizerState(&rasterizerStateDesc, &gRasterizerState);
			CHECK_HRESULT(hr);

			// Set up the viewport.
			SetupViewport(width, height);

			// Initialize shader compilation.
			//hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
			//CHECK_HRESULT(hr);

			//hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
			//CHECK_HRESULT(hr);

			// Create shaders.
			if (!CompileHLSLStringFXC(gVertexShaderSourceCode, "main", "VertexShader", SHADER_MODEL_5_VERTEX_SHADER, gVertexShaderBuffer))
			{
				LEVIATHAN_ASSERT(false);
			}

			hr = gD3D11Device->CreateVertexShader(gVertexShaderBuffer.data(), gVertexShaderBuffer.size(), nullptr, &gVertexShader);
			CHECK_HRESULT(hr);

			if (!CompileHLSLStringFXC(gPixelShaderSourceCode, "main", "PixelShader", SHADER_MODEL_5_PIXEL_SHADER, gPixelShaderBuffer))
			{
				LEVIATHAN_ASSERT(false);
			}

			hr = gD3D11Device->CreatePixelShader(static_cast<void*>(gPixelShaderBuffer.data()), gPixelShaderBuffer.size(), nullptr, &gPixelShader);
			CHECK_HRESULT(hr);

			// Create input layout.
			std::array<D3D11_INPUT_ELEMENT_DESC, 1> inputLayoutDesc =
			{
				D3D11_INPUT_ELEMENT_DESC
				{
					.SemanticName = "POSITION",
					.SemanticIndex = 0,
					.Format = DXGI_FORMAT_R32G32B32_FLOAT,
					.InputSlot = 0,
					.AlignedByteOffset = 0,
					.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
					.InstanceDataStepRate = 0
				}
			};

			hr = gD3D11Device->CreateInputLayout(inputLayoutDesc.data(), static_cast<UINT>(inputLayoutDesc.size()),
				static_cast<void*>(gVertexShaderBuffer.data()), gVertexShaderBuffer.size(), &gInputLayout);
			CHECK_HRESULT(hr);

			// Create scene resources.
			// Vertex buffer.
			std::array<VertexTypes::Vertex1Pos, 4> quadVertices =
			{
				VertexTypes::Vertex1Pos{ -0.5f, -0.5f, 0.0f }, // Bottom left.
				VertexTypes::Vertex1Pos{ -0.5f, 0.5f, 0.0f }, // Top left.
				VertexTypes::Vertex1Pos{ 0.5f, 0.5f, 0.0f }, // Top right.
				VertexTypes::Vertex1Pos{ 0.5f, -0.5f, 0.0f } // Bottom right.
			};

			D3D11_BUFFER_DESC vertexBufferDesc = {};
			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(VertexTypes::Vertex1Pos) * quadVertices.size());
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vertexBufferData = {};
			vertexBufferData.pSysMem = quadVertices.data();

			hr = gD3D11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &gVertexBuffer);
			CHECK_HRESULT(hr);

			// Index buffer.
			std::array<unsigned int, 6> quadIndices =
			{
				0, // Bottom left.
				1, // Top left.
				2, // Top right.
				0, // Bottom left.
				2, // Top right.
				3 // Bottom right.
			};

			D3D11_BUFFER_DESC indexBufferDesc = {};
			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(unsigned int) * quadIndices.size());
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA indexBufferData = {};
			indexBufferData.pSysMem = quadIndices.data();

			hr = gD3D11Device->CreateBuffer(&indexBufferDesc, &indexBufferData, &gIndexBuffer);
			CHECK_HRESULT(hr);
		}

		void ResizeWindowResources(unsigned int width, unsigned int height)
		{
			gD3D11DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

			// Release all outstanding references to the swap chain's buffers.
			gBackBufferRenderTargetView.Reset();

			HRESULT hr = {};

			// Preserve the existing buffer count and format.
			// Automatically choose the width and height to match the client rect for HWNDS.
			hr = gSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			CHECK_HRESULT(hr);

			// Get buffer and create a render target view.
			CreateBackBufferRenderTargetView();

			// Release all outstanding references to the depth stencil buffer.
			gDepthStencilBuffer.Reset();
			gDepthStencilView.Reset();

			// Recreate depth stencil buffer and view.
			CreateDepthStencilBufferAndView(width, height);

			// Set up the viewport.
			SetupViewport(width, height);
		}

		void Clear(const float* clearColor, float clearDepth, unsigned char clearStencil)
		{
			gD3D11DeviceContext->ClearRenderTargetView(gBackBufferRenderTargetView.Get(), clearColor);
			gD3D11DeviceContext->ClearDepthStencilView(gDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);

			// TODO: Refactor out of clear.
			gD3D11DeviceContext->IASetInputLayout(gInputLayout.Get());
			gD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			UINT stride = sizeof(VertexTypes::Vertex1Pos);
			UINT offset = 0;
			gD3D11DeviceContext->IASetVertexBuffers(0, 1, gVertexBuffer.GetAddressOf(), &stride, &offset);
			gD3D11DeviceContext->IASetIndexBuffer(gIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			gD3D11DeviceContext->VSSetShader(gVertexShader.Get(), nullptr, 0);
			gD3D11DeviceContext->PSSetShader(gPixelShader.Get(), nullptr, 0);

			gD3D11DeviceContext->OMSetRenderTargets(1, gBackBufferRenderTargetView.GetAddressOf(), gDepthStencilView.Get());

			gD3D11DeviceContext->DrawIndexed(6, 0, 0);
		}

		void Present()
		{
			gSwapChain->Present(((gVSync) ? 1 : 0), 0);
		}
	}
}