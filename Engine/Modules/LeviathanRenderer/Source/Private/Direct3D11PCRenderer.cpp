#include "Renderer.h"
#include "LeviathanAssert.h"
#include "VertexTypes.h"
#include "Logging.h"
#include "Serialize.h"
#include "LeviathanRenderer.h"
#include "ConstantBufferTypes.h"

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

		// RenderFrame target views of the swap chain back buffers.
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
		static constexpr const char* TARGET_SHADER_MODEL_5_VERTEX_SHADER = "vs_5_0";
		static constexpr const char* TARGET_SHADER_MODEL_5_PIXEL_SHADER = "ps_5_0";
		//static constexpr const wchar_t* SHADER_MODEL_6_VERTEX_SHADER = L"vs_6_0";
		//static constexpr const wchar_t* SHADER_MODEL_6_PIXEL_SHADER = L"ps_6_0";

		//static Microsoft::WRL::ComPtr<IDxcUtils> DxcUtils = nullptr;
		//static Microsoft::WRL::ComPtr<IDxcCompiler> DxcCompiler = nullptr;

		// Define the renderer pipeline.
		static Microsoft::WRL::ComPtr<ID3D11InputLayout> gInputLayout = {};

		static std::vector<unsigned char> gDirectionalLightVertexShaderBuffer = {};
		static Microsoft::WRL::ComPtr<ID3D11VertexShader> gDirectionalLightVertexShader = {};

		static std::vector<unsigned char> gDirectionalLightPixelShaderBuffer = {};
		static Microsoft::WRL::ComPtr<ID3D11PixelShader> gDirectionalLightPixelShader = {};

		static std::vector<unsigned char> gPointLightVertexShaderBuffer = {};
		static Microsoft::WRL::ComPtr<ID3D11VertexShader> gPointLightVertexShader = {};

		static std::vector<unsigned char> gPointLightPixelShaderBuffer = {};
		static Microsoft::WRL::ComPtr<ID3D11PixelShader> gPointLightPixelShader = {};

		static Microsoft::WRL::ComPtr<ID3D11Buffer> gDirectionalLightBuffer = {};
		static Microsoft::WRL::ComPtr<ID3D11Buffer> gPointLightBuffer = {};
		static Microsoft::WRL::ComPtr<ID3D11Buffer> gObjectBuffer = {};

		// Renderer state.
		static bool gVSync = false;

		// Scene resources.
		static std::unordered_map<RendererResourceId::IdType, Microsoft::WRL::ComPtr<ID3D11Buffer>> gVertexBuffers = {};
		static std::unordered_map<RendererResourceId::IdType, Microsoft::WRL::ComPtr<ID3D11Buffer>> gIndexBuffers = {};
		static std::unordered_map<RendererResourceId::IdType, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> gShaderResourceViews = {};
		static std::unordered_map<RendererResourceId::IdType, Microsoft::WRL::ComPtr<ID3D11SamplerState>> gSamplerStates = {};

		// Shader resource tables.
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gTexture2DSRVTable[RendererConstants::Texture2DSRVTableLength] = { nullptr };
		static Microsoft::WRL::ComPtr<ID3D11SamplerState> gTextureSamplerTable[RendererConstants::TextureSamplerTableLength] = { nullptr };

		// Macro definitions.
#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
#define LEVIATHAN_CHECK_HRESULT(hResult) LEVIATHAN_ASSERT(SUCCEEDED(hResult))
#else
#define LEVIATHAN_CHECK_HRESULT(hResult)
#endif // LEVIATHAN_BUILD_CONFIG_DEBUG.

		// Shader source code.
		//static const std::string gVertexShaderSourceCode = R"(

		//)";

		//static const std::string gPixelShaderSourceCode = R"(

		//)";

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
		//	   if (FAILED(hr)) {return false;};

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
		//	   if (FAILED(hr)) {return false;};

		//	// Check compilation result.
		//	HRESULT opResultStatus = 0;
		//	hr = opResult->GetStatus(&opResultStatus);
		//	   if (FAILED(hr)) {return false;};

		//	if (FAILED(opResultStatus))
		//	{
		//		Microsoft::WRL::ComPtr<IDxcBlobEncoding> errorBlob = nullptr;
		//		hr = opResult->GetErrorBuffer(&errorBlob);
		//		   if (FAILED(hr)) {return false;};

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
		//	   if (FAILED(hr)) {return false;};

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

		[[nodiscard]] static bool CreateBackBufferRenderTargetView()
		{
			HRESULT hr = {};

			Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer = nullptr;
			hr = gSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
			if (FAILED(hr)) { return false; };

			hr = gD3D11Device->CreateRenderTargetView(backBuffer.Get(), nullptr, &gBackBufferRenderTargetView);
			if (FAILED(hr)) { return false; };

			return true;
		}

		[[nodiscard]] static bool CreateDepthStencilBufferAndView(unsigned int width, unsigned int height)
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
			if (FAILED(hr)) { return false; };

			// Create depth stencil view.
			hr = gD3D11Device->CreateDepthStencilView(gDepthStencilBuffer.Get(), nullptr, &gDepthStencilView);
			if (FAILED(hr)) { return false; };

			return true;
		}

		[[nodiscard]] static bool CompileShaderSourceCodeString(std::string_view shaderSourceCode, std::string_view entryPoint, std::string_view name, std::string_view target,
			std::string_view cacheFile, std::vector<unsigned char>& outBuffer)
		{
			bool success = true;

			// Compile shader from source string.
			success = CompileHLSLStringFXC(shaderSourceCode, entryPoint, name, target, outBuffer);
			if (!success) { return false; }

			// Write compiled buffer to file on disk.
			success = LeviathanCore::Serialize::WriteBytesToFile(cacheFile, outBuffer);
			if (!success) { return false; }

			return success;
		}

		[[nodiscard]] static bool CreateShaderBuffer(bool forceRecompile, std::string_view shaderSourceCode, std::string_view entryPoint, std::string_view name,
			std::string_view target, std::string_view cacheFile, std::vector<unsigned char>& outBuffer)
		{
			if (forceRecompile)
			{
				LEVIATHAN_LOG("%s forced recompile.", name.data());

				return CompileShaderSourceCodeString(shaderSourceCode, entryPoint, name, target, cacheFile, outBuffer);
			}
			else
			{
				if (LeviathanCore::Serialize::FileExists(cacheFile))
				{
					LEVIATHAN_LOG("%s cache file exists. Reading file.", name.data());

					return LeviathanCore::Serialize::ReadFile(cacheFile, true, outBuffer);
				}
				else
				{
					LEVIATHAN_LOG("%s cache file does not exist. Recompiling shader source.", name.data());

					return CompileShaderSourceCodeString(shaderSourceCode, entryPoint, name, target, cacheFile, outBuffer);
				}
			}
		}

		[[nodiscard]] static bool ReadShaderSourceCodeFileContents(std::string_view file, std::vector<uint8_t>& outContentBuffer)
		{
			return LeviathanCore::Serialize::ReadFile(file, false, outContentBuffer);
		}

		[[nodiscard]] static bool CompileAndCreateShaderBuffer(std::string_view file, bool forceRecompile, std::string_view entryPoint, std::string_view name, std::string_view target,
			std::string_view cacheFile, std::vector<uint8_t>& outShaderBuffer)
		{
			bool success = true;

			// Read shader source file contents.
			std::vector<uint8_t> shaderSourceFileContents = {};
			success = ReadShaderSourceCodeFileContents(file, shaderSourceFileContents);
			if (!success) { return false; }

			// Create shader resource buffers.
			const std::string shaderSourceString = reinterpret_cast<char*>(shaderSourceFileContents.data());
			success = CreateShaderBuffer(forceRecompile, shaderSourceString, entryPoint, name, target, cacheFile, outShaderBuffer);

			return success;
		}

		[[nodiscard]] static bool InitializeShaders(bool forceRecompile)
		{
			static constexpr const char* ShaderCacheDirectory = "Shaders/";

			// Create shader directory if it does not exist.
			if (!LeviathanCore::Serialize::FileExists(ShaderCacheDirectory))
			{
				LeviathanCore::Serialize::MakeDirectory(ShaderCacheDirectory);
			}

			// Initialize shader compilation.
			//hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
			//   if (FAILED(hr)) {return false;};

			//hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
			//   if (FAILED(hr)) {return false;};

			// Compile shaders.
			bool success = true;

			const std::string directionalLightVertexShaderCacheFile(std::string(ShaderCacheDirectory) + "DirectionalLightVertexShader");
			const std::string directionalLightPixelShaderCacheFile(std::string(ShaderCacheDirectory) + "DirectionalLightPixelShader");
			const std::string pointLightVertexShaderCacheFile(std::string(ShaderCacheDirectory) + "PointLightVertexShader");
			const std::string pointLightPixelShaderCacheFile(std::string(ShaderCacheDirectory) + "PointLightPixelShader");

			success = CompileAndCreateShaderBuffer("DirectionalLightVertexShader.hlsl", forceRecompile, "main", "DirectionalLightVertexShader", 
				TARGET_SHADER_MODEL_5_VERTEX_SHADER, directionalLightVertexShaderCacheFile, gDirectionalLightVertexShaderBuffer);
			if (!success) { return false; }
			success = CompileAndCreateShaderBuffer("DirectionalLightPixelShader.hlsl", forceRecompile, "main", "DirectionalLightPixelShader",
				TARGET_SHADER_MODEL_5_PIXEL_SHADER, directionalLightPixelShaderCacheFile, gDirectionalLightPixelShaderBuffer);
			if (!success) { return false; }

			success = CompileAndCreateShaderBuffer("PointLightVertexShader.hlsl", forceRecompile, "main", "PointLightVertexShader",
				TARGET_SHADER_MODEL_5_VERTEX_SHADER, pointLightVertexShaderCacheFile, gPointLightVertexShaderBuffer);
			if (!success) { return false; }
			success = CompileAndCreateShaderBuffer("PointLightPixelShader.hlsl", forceRecompile, "main", "PointLightPixelShader",
				TARGET_SHADER_MODEL_5_PIXEL_SHADER, pointLightPixelShaderCacheFile, gPointLightPixelShaderBuffer);
			if (!success) { return false; }

			// Create shaders.
			HRESULT hr = {};

			hr = gD3D11Device->CreateVertexShader(gDirectionalLightVertexShaderBuffer.data(), gDirectionalLightVertexShaderBuffer.size(), nullptr, &gDirectionalLightVertexShader);
			if (FAILED(hr)) { return false; };
			hr = gD3D11Device->CreatePixelShader(static_cast<void*>(gDirectionalLightPixelShaderBuffer.data()), gDirectionalLightPixelShaderBuffer.size(), nullptr, &gDirectionalLightPixelShader);
			if (FAILED(hr)) { return false; };

			hr = gD3D11Device->CreateVertexShader(gPointLightVertexShaderBuffer.data(), gPointLightVertexShaderBuffer.size(), nullptr, &gPointLightVertexShader);
			if (FAILED(hr)) { return false; };
			hr = gD3D11Device->CreatePixelShader(static_cast<void*>(gPointLightPixelShaderBuffer.data()), gPointLightPixelShaderBuffer.size(), nullptr, &gPointLightPixelShader);
			if (FAILED(hr)) { return false; };

			return true;
		}

		[[nodiscard]] static bool CreateConstantBuffer(UINT byteWidth, const void* pInitialData, ID3D11Buffer** ppOutBuffer)
		{
			D3D11_BUFFER_DESC desc = {};
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			desc.ByteWidth = byteWidth;
			desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA initialData = {};
			initialData.pSysMem = pInitialData;

			return SUCCEEDED(gD3D11Device->CreateBuffer(&desc, &initialData, ppOutBuffer));
		}

		[[nodiscard]] static bool UpdateConstantBuffer(ID3D11Buffer* pBuffer, size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource = {};
			HRESULT hr = gD3D11DeviceContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(hr)) { return false; }

			memcpy(static_cast<char*>(mappedResource.pData) + byteOffsetIntoBuffer, pNewData, byteWidth);

			gD3D11DeviceContext->Unmap(pBuffer, 0);

			return true;
		}

		static D3D11_FILTER TranslateTextureSamplerFilter(const TextureSamplerFilter filter)
		{
			switch (filter)
			{
			case TextureSamplerFilter::Linear: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			case TextureSamplerFilter::Point: return D3D11_FILTER_MIN_MAG_MIP_POINT;
			default:return D3D11_FILTER();
			}
		}

		static D3D11_TEXTURE_ADDRESS_MODE TranslateTextureSamplerBorderMode(const TextureSamplerBorderMode mode)
		{
			switch (mode)
			{
			case TextureSamplerBorderMode::Wrap: return D3D11_TEXTURE_ADDRESS_WRAP;
			case TextureSamplerBorderMode::Clamp: return D3D11_TEXTURE_ADDRESS_CLAMP;
			case TextureSamplerBorderMode::SolidColor: return D3D11_TEXTURE_ADDRESS_BORDER;
			case TextureSamplerBorderMode::Mirror: return D3D11_TEXTURE_ADDRESS_MIRROR;
			default: return D3D11_TEXTURE_ADDRESS_MODE();
			}
		}

		template<typename T>
		static bool CreateDefaultConstantBuffer(ID3D11Buffer** outBuffer)
		{
			const ConstantBufferTypes::ObjectConstantBuffer initialBufferData = {};
			return CreateConstantBuffer(sizeof(T), &initialBufferData, outBuffer);
		}

		bool InitializeRendererApi(unsigned int width, unsigned int height, void* windowPlatformHandle, bool vsync, unsigned int bufferCount)
		{
			gVSync = vsync;

			HRESULT hr = {};
			bool success = true;

			// Create the DXGI factory.
			UINT createFactoryFlags = 0;
#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
			createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif //LEVIATHAN_BUILD_CONFIG_DEBUG.

			hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&gDXGIFactory));
			if (FAILED(hr)) { return false; };

			// Select adapter.
			hr = gDXGIFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&gDXGIAdapter));
			if (FAILED(hr)) { return false; };

			hr = gDXGIAdapter->GetDesc1(&gDXGIAdapterDesc);
			if (FAILED(hr)) { return false; };

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
			if (FAILED(hr)) { return false; };

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
			if (FAILED(hr)) { return false; };

			// Disable alt + enter fullscreen shortcut. Must be called after creating the swap chain.
			hr = gDXGIFactory->MakeWindowAssociation(static_cast<HWND>(windowPlatformHandle), DXGI_MWA_NO_ALT_ENTER);
			if (FAILED(hr)) { return false; };

			// Convert swap chain 1 interface to swap chain 4 interface.
			hr = swapChain1.As(&gSwapChain);
			if (FAILED(hr)) { return false; };

			// Create render target view.
			success = CreateBackBufferRenderTargetView();
			if (!success) { return false; }

			// Create the depth/stencil buffer and view.
			success = CreateDepthStencilBufferAndView(width, height);
			if (!success) { return false; }

			// Create depth stencil state.
			D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = {};
			depthStencilStateDesc.DepthEnable = TRUE;
			depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
			depthStencilStateDesc.StencilEnable = FALSE;

			hr = gD3D11Device->CreateDepthStencilState(&depthStencilStateDesc, &gDepthStencilState);
			if (FAILED(hr)) { return false; };

			// Create rasterizer state.
			D3D11_RASTERIZER_DESC rasterizerStateDesc = {};
			rasterizerStateDesc.AntialiasedLineEnable = FALSE;
			rasterizerStateDesc.CullMode = D3D11_CULL_BACK;
			rasterizerStateDesc.DepthBias = 0;
			rasterizerStateDesc.DepthBiasClamp = 0.f;
			rasterizerStateDesc.DepthClipEnable = TRUE;
			rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerStateDesc.FrontCounterClockwise = FALSE;
			rasterizerStateDesc.MultisampleEnable = FALSE;
			rasterizerStateDesc.ScissorEnable = FALSE;
			rasterizerStateDesc.SlopeScaledDepthBias = 0.f;

			hr = gD3D11Device->CreateRasterizerState(&rasterizerStateDesc, &gRasterizerState);
			if (FAILED(hr)) { return false; };

			gD3D11DeviceContext->RSSetState(gRasterizerState.Get());

			// Set up the viewport.
			SetupViewport(width, height);

			// Initialize shaders.
			bool forceRecompile = true;
#ifdef LEVIATHAN_BUILD_CONFIG_MASTER
			forceRecompile = false;
#endif

			success = InitializeShaders(forceRecompile);
			if (!success)
			{
				return false;
			}

			// Create input layout.
			std::array<D3D11_INPUT_ELEMENT_DESC, 4> inputLayoutDesc =
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
				},

				D3D11_INPUT_ELEMENT_DESC
				{
					.SemanticName = "NORMAL",
					.SemanticIndex = 0,
					.Format = DXGI_FORMAT_R32G32B32_FLOAT,
					.InputSlot = 0,
					.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
					.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
					.InstanceDataStepRate = 0
				},

				D3D11_INPUT_ELEMENT_DESC
				{
					.SemanticName = "UV",
					.SemanticIndex = 0,
					.Format = DXGI_FORMAT_R32G32_FLOAT,
					.InputSlot = 0,
					.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
					.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
					.InstanceDataStepRate = 0
				},

				D3D11_INPUT_ELEMENT_DESC
				{
					.SemanticName = "TANGENT",
					.SemanticIndex = 0,
					.Format = DXGI_FORMAT_R32G32B32_FLOAT,
					.InputSlot = 0,
					.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
					.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
					.InstanceDataStepRate = 0
				}
			};

			hr = gD3D11Device->CreateInputLayout(inputLayoutDesc.data(), static_cast<UINT>(inputLayoutDesc.size()),
				static_cast<void*>(gDirectionalLightVertexShaderBuffer.data()), gDirectionalLightVertexShaderBuffer.size(), &gInputLayout);
			if (FAILED(hr)) { return false; };

			// Create constant buffers.
			success = CreateDefaultConstantBuffer<ConstantBufferTypes::ObjectConstantBuffer>(&gObjectBuffer);
			if (!success) { return false; }
			success = CreateDefaultConstantBuffer<ConstantBufferTypes::DirectionalLightConstantBuffer>(&gDirectionalLightBuffer);
			if (!success) { return false; }
			success = CreateDefaultConstantBuffer<ConstantBufferTypes::PointLightConstantBuffer>(&gPointLightBuffer);
			if (!success) { return false; }

			return success;
		}

		bool ShutdownRendererApi()
		{
			gDXGIFactory.Reset();
			gDXGIAdapterDesc = {};
			gDXGIAdapter.Reset();

			gD3D11Device.Reset();
			gD3D11DeviceContext.Reset();
			gSwapChain.Reset();
			gFeatureLevel = {};

			gBackBufferRenderTargetView.Reset();

			gDepthStencilView.Reset();

			gDepthStencilBuffer.Reset();

			gDepthStencilState.Reset();

			gRasterizerState.Reset();
			gViewport = {};

			gInputLayout.Reset();

			gDirectionalLightVertexShaderBuffer.clear();
			gDirectionalLightVertexShader.Reset();
			gPointLightVertexShader.Reset();
			gPointLightVertexShaderBuffer.clear();

			gDirectionalLightPixelShaderBuffer.clear();
			gDirectionalLightPixelShader.Reset();
			gPointLightPixelShaderBuffer.clear();
			gPointLightPixelShader.Reset();

			gDirectionalLightBuffer.Reset();
			gPointLightBuffer.Reset();
			gObjectBuffer.Reset();

			gVertexBuffers.clear();
			gIndexBuffers.clear();
			gShaderResourceViews.clear();

			return true;
		}

		bool ResizeWindowResources(unsigned int width, unsigned int height)
		{
			gD3D11DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

			// Release all outstanding references to the swap chain's buffers.
			gBackBufferRenderTargetView.Reset();

			HRESULT hr = {};
			bool success = true;

			// Preserve the existing buffer count and format.
			// Automatically choose the width and height to match the client rect for HWNDS.
			hr = gSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			if (FAILED(hr)) { return false; };

			// Get buffer and create a render target view.
			success = CreateBackBufferRenderTargetView();
			if (!success) { return false; }

			// Release all outstanding references to the depth stencil buffer.
			gDepthStencilBuffer.Reset();
			gDepthStencilView.Reset();

			// Recreate depth stencil buffer and view.
			success = CreateDepthStencilBufferAndView(width, height);
			if (!success) { return false; }

			// Set up the viewport.
			SetupViewport(width, height);

			return success;
		}

		bool CreateVertexBuffer(const void* vertexData, unsigned int vertexCount, size_t singleVertexStrideBytes, RendererResourceId::IdType& outId)
		{
			outId = RendererResourceId::InvalidId;

			D3D11_BUFFER_DESC vertexBufferDesc = {};
			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = static_cast<UINT>(static_cast<unsigned int>(singleVertexStrideBytes) * vertexCount);
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vertexBufferData = {};
			vertexBufferData.pSysMem = vertexData;

			outId = RendererResourceId::GetAvailableId();
			gVertexBuffers.emplace(outId, nullptr);

			return SUCCEEDED(gD3D11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &gVertexBuffers.at(outId)));
		}

		bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceId::IdType& outId)
		{
			outId = RendererResourceId::InvalidId;

			D3D11_BUFFER_DESC indexBufferDesc = {};
			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(unsigned int) * indexCount);
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA indexBufferData = {};
			indexBufferData.pSysMem = indexData;

			outId = RendererResourceId::GetAvailableId();
			gIndexBuffers.emplace(outId, nullptr);

			return SUCCEEDED(gD3D11Device->CreateBuffer(&indexBufferDesc, &indexBufferData, &gIndexBuffers.at(outId)));
		}

		void DestroyVertexBuffer(RendererResourceId::IdType& resourceID)
		{
			gVertexBuffers.erase(resourceID);
			resourceID = RendererResourceId::InvalidId;
		}

		void DestroyIndexBuffer(RendererResourceId::IdType& resourceID)
		{
			gIndexBuffers.erase(resourceID);
			resourceID = RendererResourceId::InvalidId;
		}

		bool CreateTexture2D(uint32_t width, uint32_t height, const void* data, uint32_t rowPitchBytes, bool sRGB, RendererResourceId::IdType& outID)
		{
			HRESULT hr = {};

			// Create texture 2D resource.
			D3D11_TEXTURE2D_DESC texture2DDesc;
			texture2DDesc.Width = width;
			texture2DDesc.Height = height;
			texture2DDesc.MipLevels = 1;
			texture2DDesc.ArraySize = 1;
			texture2DDesc.Format = ((sRGB) ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM);
			texture2DDesc.SampleDesc.Count = 1;
			texture2DDesc.SampleDesc.Quality = 0;
			texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
			texture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texture2DDesc.CPUAccessFlags = 0;
			texture2DDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = data;
			initData.SysMemPitch = static_cast<UINT>(rowPitchBytes);

			Microsoft::WRL::ComPtr<ID3D11Texture2D> tex = nullptr;
			hr = gD3D11Device->CreateTexture2D(&texture2DDesc, &initData, &tex);
			if (FAILED(hr)) { return false; }

			// TODO: Generate mip levels.

			// Create shader resource view of texture 2D resource.
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = texture2DDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;

			outID = RendererResourceId::GetAvailableId();
			gShaderResourceViews.emplace(outID, nullptr);

			return SUCCEEDED(gD3D11Device->CreateShaderResourceView(tex.Get(), &srvDesc, gShaderResourceViews.at(outID).GetAddressOf()));
		}

		void DestroyTexture(RendererResourceId::IdType& resourceID)
		{
			gShaderResourceViews.erase(resourceID);
			resourceID = RendererResourceId::InvalidId;
		}

		bool CreateSampler(TextureSamplerFilter filter, TextureSamplerBorderMode borderMode, const float* borderColor, RendererResourceId::IdType& outId)
		{
			D3D11_SAMPLER_DESC samplerDesc = {};
			samplerDesc.Filter = TranslateTextureSamplerFilter(filter);
			samplerDesc.AddressU = TranslateTextureSamplerBorderMode(borderMode);
			samplerDesc.AddressV = TranslateTextureSamplerBorderMode(borderMode);
			samplerDesc.AddressW = TranslateTextureSamplerBorderMode(borderMode);
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			if (borderColor)
			{
				samplerDesc.BorderColor[0] = borderColor[0];
				samplerDesc.BorderColor[1] = borderColor[1];
				samplerDesc.BorderColor[2] = borderColor[2];
				samplerDesc.BorderColor[3] = borderColor[3];
			}
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.MinLOD = 0.0f;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			outId = RendererResourceId::GetAvailableId();
			gSamplerStates.emplace(outId, nullptr);

			return SUCCEEDED(gD3D11Device->CreateSamplerState(&samplerDesc, gSamplerStates.at(outId).GetAddressOf()));
		}

		void DestroySampler(RendererResourceId::IdType& resourceID)
		{
			gSamplerStates.erase(resourceID);
			resourceID = RendererResourceId::InvalidId;
		}

		void Clear(const float* clearColor, float clearDepth, unsigned char clearStencil)
		{
			gD3D11DeviceContext->ClearRenderTargetView(gBackBufferRenderTargetView.Get(), clearColor);
			gD3D11DeviceContext->ClearDepthStencilView(gDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
		}

		void SetRenderTargets()
		{
			gD3D11DeviceContext->OMSetRenderTargets(1, gBackBufferRenderTargetView.GetAddressOf(), gDepthStencilView.Get());
		}

		void SetShaderResourceTables()
		{
			gD3D11DeviceContext->PSSetShaderResources(0, RendererConstants::Texture2DSRVTableLength, gTexture2DSRVTable[0].GetAddressOf());
			gD3D11DeviceContext->PSSetSamplers(0, RendererConstants::TextureSamplerTableLength, gTextureSamplerTable[0].GetAddressOf());
		}

		void SetDirectionalLightPipeline()
		{
			gD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			gD3D11DeviceContext->IASetInputLayout(gInputLayout.Get());
			gD3D11DeviceContext->VSSetShader(gDirectionalLightVertexShader.Get(), nullptr, 0);
			gD3D11DeviceContext->PSSetShader(gDirectionalLightPixelShader.Get(), nullptr, 0);
			gD3D11DeviceContext->VSSetConstantBuffers(0, 1, gObjectBuffer.GetAddressOf());
			gD3D11DeviceContext->VSSetConstantBuffers(1, 1, gDirectionalLightBuffer.GetAddressOf());
		}

		void SetPointLightPipeline()
		{
			gD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			gD3D11DeviceContext->IASetInputLayout(gInputLayout.Get());
			gD3D11DeviceContext->VSSetShader(gPointLightVertexShader.Get(), nullptr, 0);
			gD3D11DeviceContext->PSSetShader(gPointLightPixelShader.Get(), nullptr, 0);
			gD3D11DeviceContext->VSSetConstantBuffers(0, 1, gObjectBuffer.GetAddressOf());
			gD3D11DeviceContext->VSSetConstantBuffers(1, 1, gPointLightBuffer.GetAddressOf());
		}

		void Present()
		{
			gSwapChain->Present(((gVSync) ? 1 : 0), 0);
		}

		void DrawIndexed(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceId::IdType vertexBufferId, const RendererResourceId::IdType indexBufferId)
		{
			UINT stride = static_cast<UINT>(singleVertexStrideBytes);
			UINT offset = 0;
			gD3D11DeviceContext->IASetVertexBuffers(0, 1, gVertexBuffers.at(static_cast<size_t>(vertexBufferId)).GetAddressOf(), &stride, &offset);
			gD3D11DeviceContext->IASetIndexBuffer(gIndexBuffers.at(static_cast<size_t>(indexBufferId)).Get(), DXGI_FORMAT_R32_UINT, 0);

			gD3D11DeviceContext->DrawIndexed(indexCount, 0, 0);
		}

		bool UpdateObjectBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
		{
			return UpdateConstantBuffer(gObjectBuffer.Get(), byteOffsetIntoBuffer, pNewData, byteWidth);
		}

		bool UpdateDirectionalLightBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
		{
			return UpdateConstantBuffer(gDirectionalLightBuffer.Get(), byteOffsetIntoBuffer, pNewData, byteWidth);
		}

		bool UpdatePointLightBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
		{
			return UpdateConstantBuffer(gPointLightBuffer.Get(), byteOffsetIntoBuffer, pNewData, byteWidth);
		}

		void SetColorTexture2DResource(RendererResourceId::IdType texture2DId)
		{
			gTexture2DSRVTable[RendererConstants::ColorTexture2DSRVTableIndex] = gShaderResourceViews.at(texture2DId);
		}

		void SetRoughnessTexture2DResource(RendererResourceId::IdType texture2DId)
		{
			gTexture2DSRVTable[RendererConstants::RoughnessTexture2DSRVTableIndex] = gShaderResourceViews.at(texture2DId);
		}

		void SetMetallicTexture2DResource(RendererResourceId::IdType texture2DId)
		{
			gTexture2DSRVTable[RendererConstants::MetallicTexture2DSRVTableIndex] = gShaderResourceViews.at(texture2DId);
		}

		void SetNormalTexture2DResource(RendererResourceId::IdType texture2DId)
		{
			gTexture2DSRVTable[RendererConstants::NormalTexture2DSRVTableIndex] = gShaderResourceViews.at(texture2DId);
		}

		void SetColorTextureSampler(RendererResourceId::IdType samplerId)
		{
			gTextureSamplerTable[RendererConstants::ColorTextureSamplerTableIndex] = gSamplerStates.at(samplerId);
		}

		void SetRoughnessTextureSampler(RendererResourceId::IdType samplerId)
		{
			gTextureSamplerTable[RendererConstants::RoughnessTextureSamplerTableIndex] = gSamplerStates.at(samplerId);
		}

		void SetMetallicTextureSampler(RendererResourceId::IdType samplerId)
		{
			gTextureSamplerTable[RendererConstants::MetallicTextureSamplerTableIndex] = gSamplerStates.at(samplerId);
		}

		void SetNormalTextureSampler(RendererResourceId::IdType samplerId)
		{
			gTextureSamplerTable[RendererConstants::NormalTextureSamplerTableIndex] = gSamplerStates.at(samplerId);
		}

#ifdef LEVIATHAN_WITH_TOOLS
		bool ImGuiRendererInitialize()
		{
			return ImGui_ImplDX11_Init(gD3D11Device.Get(), gD3D11DeviceContext.Get());
		}

		void ImGuiRendererShutdown()
		{
			ImGui_ImplDX11_Shutdown();
		}

		void ImGuiRendererNewFrame()
		{
			ImGui_ImplDX11_NewFrame();
		}

		void ImGuiRender()
		{
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
#endif // LEVIATHAN_WITH_TOOLS.
	}
}