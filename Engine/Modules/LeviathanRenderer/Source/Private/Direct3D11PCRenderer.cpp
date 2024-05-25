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

		static Microsoft::WRL::ComPtr<ID3D11Buffer> gSceneBuffer = {};
		static Microsoft::WRL::ComPtr<ID3D11Buffer> gObjectBuffer = {};
		static Microsoft::WRL::ComPtr<ID3D11Buffer> gMaterialBuffer = {};

		// Renderer state.
		static bool gVSync = false;

		// Scene resources.
		static std::unordered_map<RendererResourceID::IDType, Microsoft::WRL::ComPtr<ID3D11Buffer>> gVertexBuffers = {};
		static std::unordered_map<RendererResourceID::IDType, Microsoft::WRL::ComPtr<ID3D11Buffer>> gIndexBuffers = {};
		static std::unordered_map<RendererResourceID::IDType, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> gShaderResourceViews = {};

		// Samplers.
		static Microsoft::WRL::ComPtr<ID3D11SamplerState> gSamplerState = {};

		// Macro definitions.
#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
#define CHECK_HRESULT(hResult) LEVIATHAN_ASSERT(SUCCEEDED(hResult))
#else
#define CHECK_HRESULT(hResult)
#endif // LEVIATHAN_BUILD_CONFIG_DEBUG.

		// Shader source code.
		static const std::string gVertexShaderSourceCode = R"(
cbuffer ObjectBuffer : register(b0)
{
    float4x4 WorldViewMatrix;
    float4x4 WorldViewProjectionMatrix;
    float4x4 NormalMatrix;
};

struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 UV : UV;
};

struct VertexOutput
{
    float4 PositionClipSpace : SV_POSITION;
    float3 PositionViewSpace : POSITION_VIEW_SPACE;
    float3 InterpolatedNormalViewSpace : INTERPOLATED_NORMAL_VIEW_SPACE;
    float2 TexCoord : TEXTURE_COORD;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.PositionClipSpace = mul(WorldViewProjectionMatrix, float4(input.Position, 1.0f));
    output.PositionViewSpace = mul(WorldViewMatrix, float4(input.Position, 1.0f)).xyz;
    output.InterpolatedNormalViewSpace = normalize(mul(NormalMatrix, float4(input.Normal, 0.0f)).xyz);
    output.TexCoord = input.UV;
    return output;
}
		)";

		static const std::string gPixelShaderSourceCode = R"(
// Renderer constants.
#define MAX_DIRECTIONAL_LIGHT_COUNT 10
#define MAX_POINT_LIGHT_COUNT 10
#define MAX_SPOT_LIGHT_COUNT 10

// Shader definitions.
#define PI 3.14159265359

struct DirectionalLight
{
    float3 Radiance;
    float3 DirectionViewSpace;
};

struct PointLight
{
    float3 Radiance;
    float3 PositionViewSpace;
};

struct SpotLight
{
    float3 Radiance;
    float3 PositionViewSpace;
    float3 DirectionViewSpace;
    float CosineInnerConeAngle;
    float CosineOuterConeAngle;
};

struct PixelInput
{
    float4 PositionClipSpace : SV_POSITION;
    float3 PositionViewSpace : POSITION_VIEW_SPACE;
    float3 InterpolatedNormalViewSpace : INTERPOLATED_NORMAL_VIEW_SPACE;
    float2 TexCoord : TEXTURE_COORD;
};

cbuffer SceneBuffer : register(b0)
{
    uint DirectionalLightCount;
    uint PointLightCount;
    uint SpotLightCount;
    DirectionalLight DirectionalLights[MAX_DIRECTIONAL_LIGHT_COUNT];
    PointLight PointLights[MAX_POINT_LIGHT_COUNT];
    SpotLight SpotLights[MAX_SPOT_LIGHT_COUNT];
}

cbuffer MaterialBuffer : register(b1)
{
    float3 Color;
    float Roughness;
    float Metallic;
};

Texture2D Texture : register(t0);
SamplerState XSamplerState : register(s0);

float Square(float x)
{
    return x * x;
}

float Attenuation(float distance)
{
    return 1.0f / Square(distance);
}

float3 Lambert(float3 color)
{
    return color / PI;
}

float3 DiffuseBRDF(float kD, float3 lambert)
{
    return kD * lambert;
}

float TrowbridgeReitzGGX(float roughness, float3 halfVector, float3 surfaceNormal)
{
    float roughnessSquared = Square(roughness) * Square(roughness); // Roughness is raised to the power of 4 instead of 2 based on the experience of Disney and Epic Games.
    return roughnessSquared / (PI * Square((Square(dot(surfaceNormal, halfVector)) * (roughnessSquared - 1) + 1)));
}

float SchlickGGX(float nDotV, float k)
{
    return nDotV / (nDotV * (1.0f - k) + k);
}

float Smith(float nDotV, float nDotL, float roughness)
{
    float k = Square(roughness + 1.0f) / 8.0f;
    return SchlickGGX(nDotV, k) * SchlickGGX(nDotL, k);
}

float3 SchlickFresnel(float vDotH, float3 surfaceColor, float metallic)
{
    float3 f0 = float3(0.04f, 0.04f, 0.04f);
    f0 = lerp(f0, surfaceColor, metallic);
    return f0 + (1.0f - f0) * pow(saturate(1.0f - vDotH), 5.0f);
}

float3 CalculateLighting(float3 surfaceToLightDirection, float3 surfaceToViewDirection, float3 surfaceNormal, float nDotV, float3 radiance, float3 baseColor, float roughness, float metallic)
{
    float3 halfDirectionViewSpace = normalize(surfaceToViewDirection + surfaceToLightDirection);
        
    float nDotH = saturate(dot(surfaceNormal, halfDirectionViewSpace));
    float vDotH = saturate(dot(surfaceToViewDirection, halfDirectionViewSpace));
    float nDotL = saturate(dot(surfaceNormal, surfaceToLightDirection));

    float3 f = SchlickFresnel(vDotH, baseColor, metallic);
    float3 kS = f;
    float3 kD = 1.0f - kS;
        
    float3 diffuse = kD * Lambert(baseColor);
        
    float d = TrowbridgeReitzGGX(roughness, halfDirectionViewSpace, surfaceNormal);
    float g = Smith(nDotV, nDotL, roughness);
    float3 specular = (d * g * f) / ((4.0f * nDotL * nDotV) + 0.0001f);

    return (diffuse + specular) * radiance * nDotL;
}

float4 main(PixelInput input) : SV_TARGET
{
    float3 textureColor = Texture.Sample(XSamplerState, input.TexCoord);

    // HDR tone mapping.
    textureColor = textureColor / (textureColor + float3(1.0f, 1.0f, 1.0f));

    // Gamma correction.
    float gammaExponent = 1.0f / 2.2f;
    float4 finalColor = float4(pow(textureColor, float3(gammaExponent, gammaExponent, gammaExponent)), 1.0f);

    return finalColor;
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

					return LeviathanCore::Serialize::ReadBytesFromFile(cacheFile, outBuffer);
				}
				else
				{
					LEVIATHAN_LOG("%s cache file does not exist. Recompiling shader source.", name.data());

					return CompileShaderSourceCodeString(shaderSourceCode, entryPoint, name, target, cacheFile, outBuffer);
				}
			}
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

			std::string VertexShaderCacheFile(ShaderCacheDirectory);
			VertexShaderCacheFile += "VertexShader";

			std::string PixelShaderCacheFile(ShaderCacheDirectory);
			PixelShaderCacheFile += "PixelShader";

			success = CreateShaderBuffer(forceRecompile, gVertexShaderSourceCode, "main", "VertexShader", SHADER_MODEL_5_VERTEX_SHADER, VertexShaderCacheFile, gVertexShaderBuffer);
			if (!success) { return false; }

			success = CreateShaderBuffer(forceRecompile, gPixelShaderSourceCode, "main", "PixelShader", SHADER_MODEL_5_PIXEL_SHADER, PixelShaderCacheFile, gPixelShaderBuffer);
			if (!success) { return false; }

			// Create shaders.
			HRESULT hr = {};

			hr = gD3D11Device->CreateVertexShader(gVertexShaderBuffer.data(), gVertexShaderBuffer.size(), nullptr, &gVertexShader);
			if (FAILED(hr)) { return false; };

			hr = gD3D11Device->CreatePixelShader(static_cast<void*>(gPixelShaderBuffer.data()), gPixelShaderBuffer.size(), nullptr, &gPixelShader);
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

			// Create pipelines.
			// Create input layout.
			std::array<D3D11_INPUT_ELEMENT_DESC, 3> inputLayoutDesc =
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
				}
			};

			hr = gD3D11Device->CreateInputLayout(inputLayoutDesc.data(), static_cast<UINT>(inputLayoutDesc.size()),
				static_cast<void*>(gVertexShaderBuffer.data()), gVertexShaderBuffer.size(), &gInputLayout);
			if (FAILED(hr)) { return false; };

			// Create constant buffers.
			// Vertex shader
			// Object constant buffer.
			ConstantBufferTypes::ObjectConstantBuffer initialObjectBufferData = {};
			success = CreateConstantBuffer(sizeof(ConstantBufferTypes::ObjectConstantBuffer), &initialObjectBufferData, &gObjectBuffer);
			if (!success) { return false; }

			// Pixel shader.
			// Scene constant buffer.
			ConstantBufferTypes::SceneConstantBuffer initialSceneBufferData = {};
			success = CreateConstantBuffer(sizeof(ConstantBufferTypes::SceneConstantBuffer), &initialSceneBufferData, &gSceneBuffer);
			if (!success) { return false; }

			// Material constant buffer.
			ConstantBufferTypes::MaterialConstantBuffer initialMaterialBufferData = {};
			success = CreateConstantBuffer(sizeof(ConstantBufferTypes::MaterialConstantBuffer), static_cast<const void*>(&initialMaterialBufferData), &gMaterialBuffer);
			if (!success) { return false; }

			// Set pipeline primitive topology.
			gD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// Create samplers.
			D3D11_SAMPLER_DESC samplerDesc = {};
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.MinLOD = 0.0f;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			hr = gD3D11Device->CreateSamplerState(&samplerDesc, gSamplerState.GetAddressOf());
			if (FAILED(hr)) { return false; };

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

			gVertexShaderBuffer.clear();
			gVertexShader.Reset();

			gPixelShaderBuffer.clear();
			gPixelShader.Reset();

			gSceneBuffer.Reset();
			gObjectBuffer.Reset();
			gMaterialBuffer.Reset();

			gVertexBuffers.clear();
			gIndexBuffers.clear();

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

		bool CreateVertexBuffer(const void* vertexData, unsigned int vertexCount, size_t singleVertexStrideBytes, RendererResourceID::IDType& outId)
		{
			outId = RendererResourceID::InvalidID;

			D3D11_BUFFER_DESC vertexBufferDesc = {};
			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = static_cast<UINT>(static_cast<unsigned int>(singleVertexStrideBytes) * vertexCount);
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vertexBufferData = {};
			vertexBufferData.pSysMem = vertexData;

			outId = RendererResourceID::GetAvailableID();
			gVertexBuffers.emplace(outId, nullptr);

			return SUCCEEDED(gD3D11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &gVertexBuffers[outId]));
		}

		bool CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceID::IDType& outId)
		{
			outId = RendererResourceID::InvalidID;

			D3D11_BUFFER_DESC indexBufferDesc = {};
			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(unsigned int) * indexCount);
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA indexBufferData = {};
			indexBufferData.pSysMem = indexData;

			outId = RendererResourceID::GetAvailableID();
			gIndexBuffers.emplace(outId, nullptr);

			return SUCCEEDED(gD3D11Device->CreateBuffer(&indexBufferDesc, &indexBufferData, &gIndexBuffers[outId]));
		}

		void DestroyVertexBuffer(RendererResourceID::IDType& resourceID)
		{
			gVertexBuffers.erase(resourceID);
			resourceID = RendererResourceID::InvalidID;
		}

		void DestroyIndexBuffer(RendererResourceID::IDType& resourceID)
		{
			gIndexBuffers.erase(resourceID);
			resourceID = RendererResourceID::InvalidID;
		}

		bool CreateTexture2D(uint32_t width, uint32_t height, const void* data, uint32_t rowPitchBytes, bool sRGB, RendererResourceID::IDType& outID)
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

			outID = RendererResourceID::GetAvailableID();
			gShaderResourceViews.emplace(outID, nullptr);

			return SUCCEEDED(gD3D11Device->CreateShaderResourceView(tex.Get(), &srvDesc, gShaderResourceViews[outID].GetAddressOf()));
		}

		void DestroyTexture(RendererResourceID::IDType& resourceID)
		{
			gShaderResourceViews.erase(resourceID);
			resourceID = RendererResourceID::InvalidID;
		}

		void Clear(const float* clearColor, float clearDepth, unsigned char clearStencil)
		{
			gD3D11DeviceContext->ClearRenderTargetView(gBackBufferRenderTargetView.Get(), clearColor);
			gD3D11DeviceContext->ClearDepthStencilView(gDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
		}

		void BeginRenderPass()
		{
			gD3D11DeviceContext->IASetInputLayout(gInputLayout.Get());

			gD3D11DeviceContext->OMSetRenderTargets(1, gBackBufferRenderTargetView.GetAddressOf(), gDepthStencilView.Get());

			gD3D11DeviceContext->VSSetShader(gVertexShader.Get(), nullptr, 0);
			gD3D11DeviceContext->PSSetShader(gPixelShader.Get(), nullptr, 0);

			gD3D11DeviceContext->VSSetConstantBuffers(0, 1, gObjectBuffer.GetAddressOf());
			gD3D11DeviceContext->PSSetConstantBuffers(0, 1, gSceneBuffer.GetAddressOf());
			gD3D11DeviceContext->PSSetConstantBuffers(1, 1, gMaterialBuffer.GetAddressOf());

			// TODO: Implement bindless texture strategy.
			gD3D11DeviceContext->PSSetShaderResources(0, 1, gShaderResourceViews.at(3).GetAddressOf());
			gD3D11DeviceContext->PSSetSamplers(0, 1, gSamplerState.GetAddressOf());
		}

		void Present()
		{
			gSwapChain->Present(((gVSync) ? 1 : 0), 0);
		}

		void DrawIndexed(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceID::IDType vertexBufferId, const RendererResourceID::IDType indexBufferId)
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

		bool UpdateSceneBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
		{
			return UpdateConstantBuffer(gSceneBuffer.Get(), byteOffsetIntoBuffer, pNewData, byteWidth);
		}

		bool UpdateMaterialBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
		{
			return UpdateConstantBuffer(gMaterialBuffer.Get(), byteOffsetIntoBuffer, pNewData, byteWidth);
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