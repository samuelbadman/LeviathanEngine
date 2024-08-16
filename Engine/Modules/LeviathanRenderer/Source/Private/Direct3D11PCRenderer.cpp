#include "Renderer.h"
#include "LeviathanAssert.h"
#include "VertexTypes.h"
#include "Logging.h"
#include "Serialize.h"
#include "LeviathanRenderer.h"
#include "ConstantBufferTypes.h"

// Temp.
#include "LinearColor.h"

namespace LeviathanRenderer
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

	// Texture resource used to render the scene to and sampled during post processing.
	static Microsoft::WRL::ComPtr<ID3D11Texture2D> gSceneTextureResource = {};

	// Scene texture render target view.
	static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> gSceneTextureRenderTargetView = {};

	// Scene texture shader resource view.
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gSceneTextureShaderResourceView = {};

	// Scene texture sampler state.
	static Microsoft::WRL::ComPtr<ID3D11SamplerState> gSceneTextureSamplerState = {};

	// Shader compilation settings.
	static constexpr const char* SHADER_MODEL_5_VERTEX_SHADER = "vs_5_0";
	static constexpr const char* SHADER_MODEL_5_PIXEL_SHADER = "ps_5_0";
	static constexpr const char* CompiledShaderCacheDirectory = "Shaders/";
	static constexpr char CompiledShaderCacheFileTypeSeparatingCharacter = '_';
	static constexpr const char* CompiledShaderCacheVertexShaderFilePostFixString = "CompiledVertexShader";
	static constexpr const char* CompiledShaderCachePixelShaderFilePostFixString = "CompiledPixelShader";
	static constexpr bool ForceShaderRecompilation =
#ifdef LEVIATHAN_BUILD_CONFIG_MASTER
		false;
#else
		true;
#endif

	// Note: For d3d12 renderer implementation.
	//static constexpr const wchar_t* SHADER_MODEL_6_VERTEX_SHADER = L"vs_6_0";
	//static constexpr const wchar_t* SHADER_MODEL_6_PIXEL_SHADER = L"ps_6_0";
	//static Microsoft::WRL::ComPtr<IDxcUtils> DxcUtils = nullptr;
	//static Microsoft::WRL::ComPtr<IDxcCompiler> DxcCompiler = nullptr;

	// Shader compilation helper functions and types.
	struct ShaderDescription
	{
		// Path to the file containing the shader source code.
		std::string_view SourceCodeFile = {};
		// The name of the entry point function in the shader.
		std::string_view EntryPointName = {};
		// Null terminated list of shader definitions.
		const D3D_SHADER_MACRO* ShaderMacros = nullptr;
	};

	static bool ReadShaderSourceCodeFileContents(std::string_view file, std::vector<uint8_t>& outBuffer)
	{
		return LeviathanCore::Serialize::ReadFile(file, false, outBuffer);
	}

	static bool ReadShaderSourceCodeFileContentsToString(std::string_view file, std::string& outString)
	{
		std::vector<uint8_t> shaderSourceCodeBuffer = {};
		if (!ReadShaderSourceCodeFileContents(file, shaderSourceCodeBuffer))
		{
			return false;
		}
		outString = reinterpret_cast<char*>(shaderSourceCodeBuffer.data());
		return true;
	}

	static bool CompileHLSLStringFXC(const std::string_view string, const std::string_view entryPoint, const std::string_view name, const D3D_SHADER_MACRO* shaderMacros,
		const std::string_view target, std::vector<unsigned char>& outBuffer)
	{
		HRESULT hr = {};

		Microsoft::WRL::ComPtr<ID3DBlob> blob = {};
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = {};
		hr = D3DCompile(static_cast<const void*>(string.data()), string.size(), name.data(), shaderMacros, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.data(),
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

	// D3D12 DXC shader compilation example.
	/*// Direct3D11 only supports shaders compiled with FXC which can only compile shader model 5 and below. Shader model 6 and above shaders must be compiled with
	// DXC and can only be used with Direct3D12.
	// Note: For d3d12 renderer implementation.
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
	*/

	static bool CompileShaderSourceCodeString(std::string_view shaderSourceCode, std::string_view entryPoint, std::string_view name, const D3D_SHADER_MACRO* shaderMacros,
		std::string_view target, std::vector<unsigned char>& outBuffer)
	{
		return CompileHLSLStringFXC(shaderSourceCode, entryPoint, name, shaderMacros, target, outBuffer);
	}

	static bool CompileNewShader(std::string_view sourceCodeFile, std::string_view entryPointName, std::string_view name, const D3D_SHADER_MACRO* shaderMacros,
		std::string_view target, std::string_view cacheFile, std::vector<uint8_t>& outBuffer)
	{
		// Get shader source code as string.
		std::string sourceCodeString = {};
		if (!ReadShaderSourceCodeFileContentsToString(sourceCodeFile, sourceCodeString))
		{
			return false;
		}

		// Compile source code string and store in buffer.
		if (!CompileShaderSourceCodeString(sourceCodeString, entryPointName, name, shaderMacros, target, outBuffer))
		{
			return false;
		}

		// Write compiled shader buffer to file on disk.
		if (!LeviathanCore::Serialize::WriteBytesToFile(cacheFile, outBuffer))
		{
			return false;
		}

		return true;
	}

	static bool DoesCompiledShaderCacheFileExist(std::string_view file)
	{
		return LeviathanCore::Serialize::FileExists(file);
	}

	static bool ReadCompiledShaderCacheFile(std::string_view file, std::vector<uint8_t>& outBuffer)
	{
		return LeviathanCore::Serialize::ReadFile(file, true, outBuffer);
	}

	static bool TryReadCompiledShaderCacheFileElseCompileNewShader(std::string_view compiledShaderCacheFile, std::string_view shaderSourceCodeFile,
		std::string_view entryPointName, std::string_view shaderName, const D3D_SHADER_MACRO* shaderMacros, std::string_view shaderTarget, std::vector<uint8_t>& outBuffer)
	{
		if (DoesCompiledShaderCacheFileExist(compiledShaderCacheFile))
		{
			LEVIATHAN_LOG("%s compiled shader cache file exists. Reading file.", compiledShaderCacheFile.data());
			return ReadCompiledShaderCacheFile(compiledShaderCacheFile, outBuffer);
		}
		else
		{
			LEVIATHAN_LOG("%s compiled shader cache file does not exist. Recompiling shader source.", compiledShaderCacheFile.data());
			return CompileNewShader(shaderSourceCodeFile, entryPointName, shaderName, shaderMacros, shaderTarget, compiledShaderCacheFile, outBuffer);
		}
	}

	class Pipeline
	{
	private:
		static constexpr char PipelineShaderNameShaderTypeSeparatingCharacter = '_';
		static constexpr const char* PipelineVertexShaderNamePostFixString = "VertexShader";
		static constexpr const char* PipelinePixelShaderNamePostFixString = "PixelShader";

		Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader = {};
		Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout = {};
		Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader = {};

	public:
		Pipeline() = default;
		Pipeline(std::string_view name, const ShaderDescription& vertexShaderDescription, const D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements,
			const ShaderDescription& pixelShaderDescription)
		{
			if (!Create(name, vertexShaderDescription, inputElementDescs, numElements, pixelShaderDescription))
			{
				LEVIATHAN_LOG("Failed to create %s rendering pipeline.", name.data());
			}
		}

		bool Create(std::string_view name, const ShaderDescription& vertexShaderDescription, const D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements,
			const ShaderDescription& pixelShaderDescription)
		{
			// Compile shaders.
			// If previously compiled shader file exists, read its contents into the buffer and use that instead of recompiling the source code.
			const std::string compiledVertexShaderCacheFile(std::string(CompiledShaderCacheDirectory) +
				name.data() + CompiledShaderCacheFileTypeSeparatingCharacter + CompiledShaderCacheVertexShaderFilePostFixString);
			const std::string compiledPixelShaderCacheFile(std::string(CompiledShaderCacheDirectory) +
				name.data() + CompiledShaderCacheFileTypeSeparatingCharacter + CompiledShaderCachePixelShaderFilePostFixString);
			const std::string vertexShaderName(std::string(name.data()) + PipelineShaderNameShaderTypeSeparatingCharacter + PipelineVertexShaderNamePostFixString);
			const std::string pixelShaderName(std::string(name.data()) + PipelineShaderNameShaderTypeSeparatingCharacter + PipelinePixelShaderNamePostFixString);

			std::vector<uint8_t> compiledVertexShader = {};
			std::vector<uint8_t> compiledPixelShader = {};

			if (ForceShaderRecompilation)
			{
				LEVIATHAN_LOG("Forced recompilation of shaders for %s pipeline.", name.data());
				if (!CompileNewShader(vertexShaderDescription.SourceCodeFile, vertexShaderDescription.EntryPointName, name, vertexShaderDescription.ShaderMacros,
					SHADER_MODEL_5_VERTEX_SHADER, compiledVertexShaderCacheFile, compiledVertexShader))
				{
					return false;
				}
				if (!CompileNewShader(pixelShaderDescription.SourceCodeFile, pixelShaderDescription.EntryPointName, name, pixelShaderDescription.ShaderMacros,
					SHADER_MODEL_5_PIXEL_SHADER, compiledPixelShaderCacheFile, compiledPixelShader))
				{
					return false;
				}
			}
			else
			{
				if (!TryReadCompiledShaderCacheFileElseCompileNewShader(compiledVertexShaderCacheFile, vertexShaderDescription.SourceCodeFile, vertexShaderDescription.EntryPointName,
					vertexShaderName, vertexShaderDescription.ShaderMacros, SHADER_MODEL_5_VERTEX_SHADER, compiledVertexShader))
				{
					return false;
				}
				if (!TryReadCompiledShaderCacheFileElseCompileNewShader(compiledPixelShaderCacheFile, pixelShaderDescription.SourceCodeFile, pixelShaderDescription.EntryPointName,
					pixelShaderName, pixelShaderDescription.ShaderMacros, SHADER_MODEL_5_PIXEL_SHADER, compiledPixelShader))
				{
					return false;
				}
			}

			// Create shaders.
			HRESULT hr = {};
			hr = gD3D11Device->CreateVertexShader(static_cast<const void*>(compiledVertexShader.data()), compiledVertexShader.size(), nullptr, &VertexShader);
			if (FAILED(hr)) { return false; };
			hr = gD3D11Device->CreatePixelShader(static_cast<void*>(compiledPixelShader.data()), compiledPixelShader.size(), nullptr, &PixelShader);
			if (FAILED(hr)) { return false; };

			// Create input layout.
			hr = gD3D11Device->CreateInputLayout(inputElementDescs, numElements,
				static_cast<void*>(compiledVertexShader.data()), compiledVertexShader.size(), &InputLayout);
			if (FAILED(hr)) { return false; };

			return true;
		}

		void Destroy()
		{
			VertexShader.Reset();
			InputLayout.Reset();
			PixelShader.Reset();
		}

		inline ID3D11InputLayout* GetInputLayout() const { return InputLayout.Get(); }
		inline ID3D11VertexShader* GetVertexShader() const { return VertexShader.Get(); }
		inline ID3D11PixelShader* GetPixelShader() const { return PixelShader.Get(); }
	};

	// Depth/stencil states.
	static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> gDepthStencilStateWriteDepthDepthFuncLessStencilDisabled = {};
	static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> gDepthStencilStateNoWriteDepthDepthFuncEqualStencilDisabled = {};
	static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> gDepthStencilStateNoWriteDepthDepthFuncLessStencilDisabled = {};
	static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> gDepthStencilStateDepthStencilDisabled = {};

	// Blend states.
	static Microsoft::WRL::ComPtr<ID3D11BlendState> gBlendStateAdditive = {};

	// Rasterizer states.
	static Microsoft::WRL::ComPtr<ID3D11RasterizerState> gRasterizerState = {};

	// Viewport.
	static D3D11_VIEWPORT gViewport = {};

	// Pipelines.
	static Pipeline gEquirectangularToCubemapPipeline = {};
	static Pipeline gSkyboxPipeline = {};
	static Pipeline gEnvironmentLightPipeline = {};
	static Pipeline gDirectionalLightPipeline = {};
	static Pipeline gPointLightPipeline = {};
	static Pipeline gSpotLightPipeline = {};
	static Pipeline gPostProcessPipeline = {};

	// Renderer state.
	static bool gVSync = false;

	// Scene resources.
	static std::unordered_map<RendererResourceId::IdType, Microsoft::WRL::ComPtr<ID3D11Buffer>> gVertexBuffers = {};
	static std::unordered_map<RendererResourceId::IdType, Microsoft::WRL::ComPtr<ID3D11Buffer>> gIndexBuffers = {};
	static std::unordered_map<RendererResourceId::IdType, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> gShaderResourceViews = {};
	static std::unordered_map<RendererResourceId::IdType, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> gRenderTargetViews = {};
	static std::unordered_map<RendererResourceId::IdType, Microsoft::WRL::ComPtr<ID3D11SamplerState>> gSamplerStates = {};

	// Shader constant buffers.
	static Microsoft::WRL::ComPtr<ID3D11Buffer> gEquirectangularToCubemapBuffer = {};
	static Microsoft::WRL::ComPtr<ID3D11Buffer> gSkyboxBuffer = {};
	static Microsoft::WRL::ComPtr<ID3D11Buffer> gDirectionalLightBuffer = {};
	static Microsoft::WRL::ComPtr<ID3D11Buffer> gPointLightBuffer = {};
	static Microsoft::WRL::ComPtr<ID3D11Buffer> gSpotLightBuffer = {};
	static Microsoft::WRL::ComPtr<ID3D11Buffer> gObjectBuffer = {};

	// Shader resource tables.
	static std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, RendererConstants::Texture2DSRVTableLength> gTexture2DSRVTable = { nullptr };
	static std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, RendererConstants::TextureCubeSRVTableLength> gTextureCubeSRVTable = { nullptr };
	static std::array<Microsoft::WRL::ComPtr<ID3D11SamplerState>, RendererConstants::TextureSamplerTableLength> gTextureSamplerTable = { nullptr };

	// Macro definitions.
#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
#define LEVIATHAN_CHECK_HRESULT(hResult) LEVIATHAN_ASSERT(SUCCEEDED(hResult))
#else
#define LEVIATHAN_CHECK_HRESULT(hResult)
#endif // LEVIATHAN_BUILD_CONFIG_DEBUG.

	static bool CreateBackBufferRenderTargetView()
	{
		HRESULT hr = {};

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer = nullptr;
		hr = gSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
		if (FAILED(hr)) { return false; };

		hr = gD3D11Device->CreateRenderTargetView(backBuffer.Get(), nullptr, &gBackBufferRenderTargetView);
		if (FAILED(hr)) { return false; };

		return true;
	}

	static bool CreateDepthStencilBufferAndView(unsigned int width, unsigned int height)
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

	static bool CreateConstantBuffer(UINT byteWidth, const void* pInitialData, ID3D11Buffer** ppOutBuffer)
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

	template<typename T>
	static bool CreateDefaultConstantBuffer(ID3D11Buffer** outBuffer)
	{
		const T initialBufferData = {};
		return CreateConstantBuffer(sizeof(T), &initialBufferData, outBuffer);
	}

	static bool UpdateConstantBuffer(ID3D11Buffer* pBuffer, size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
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
		case TextureSamplerFilter::Anisotropic: return D3D11_FILTER_ANISOTROPIC;
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

	static bool CreateSceneTextureResource(UINT width, UINT height)
	{
		HRESULT hr = {};

		D3D11_TEXTURE2D_DESC sceneTextureResourceDesc = {};
		sceneTextureResourceDesc.Width = width;
		sceneTextureResourceDesc.Height = height;
		sceneTextureResourceDesc.MipLevels = 1;
		sceneTextureResourceDesc.ArraySize = 1;
		sceneTextureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		sceneTextureResourceDesc.SampleDesc.Count = 1;
		sceneTextureResourceDesc.Usage = D3D11_USAGE_DEFAULT;
		sceneTextureResourceDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		sceneTextureResourceDesc.CPUAccessFlags = 0;
		sceneTextureResourceDesc.MiscFlags = 0;

		hr = gD3D11Device->CreateTexture2D(&sceneTextureResourceDesc, nullptr, &gSceneTextureResource);
		if (FAILED(hr)) { return false; }

		// Create scene texture render target view.
		D3D11_RENDER_TARGET_VIEW_DESC sceneTextureRenderTargetViewDesc = {};
		sceneTextureRenderTargetViewDesc.Format = sceneTextureResourceDesc.Format;
		sceneTextureRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		sceneTextureRenderTargetViewDesc.Texture2D.MipSlice = 0;

		hr = gD3D11Device->CreateRenderTargetView(gSceneTextureResource.Get(), &sceneTextureRenderTargetViewDesc, &gSceneTextureRenderTargetView);
		if (FAILED(hr)) { return false; }

		// Create scene texture shader resource view.
		D3D11_SHADER_RESOURCE_VIEW_DESC sceneTextureShaderResourceViewDesc = {};
		sceneTextureShaderResourceViewDesc.Format = sceneTextureResourceDesc.Format;
		sceneTextureShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		sceneTextureShaderResourceViewDesc.Texture2D.MipLevels = 1;
		sceneTextureShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

		hr = gD3D11Device->CreateShaderResourceView(gSceneTextureResource.Get(), &sceneTextureShaderResourceViewDesc, &gSceneTextureShaderResourceView);
		if (FAILED(hr)) { return false; }

		return true;
	}

	static uint32_t GetNumMipLevels(uint32_t width, uint32_t height)
	{
		uint32_t numLevels = 1;
		while (width > 1 && height > 1)
		{
			width = LeviathanCore::MathLibrary::Max(width / 2u, 1u);
			height = LeviathanCore::MathLibrary::Max(height / 2u, 1u);
			++numLevels;
		}
		return numLevels;
	}

	static bool CreatePipelines()
	{
		// Equirectangular to cubemap.
		static constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 1> equirectToCubemapInputLayoutDesc =
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

		bool success = gEquirectangularToCubemapPipeline.Create("EquirectToCubemapPipeline", { .SourceCodeFile = "EquirectToCubemapVertexShader.hlsl", .EntryPointName = "main", .ShaderMacros = nullptr },
			equirectToCubemapInputLayoutDesc.data(), static_cast<UINT>(equirectToCubemapInputLayoutDesc.size()), { .SourceCodeFile = "EquirectToCubemapPixelShader.hlsl", .EntryPointName = "main", .ShaderMacros = nullptr });
		if (!success) { return false; }

		// Skybox.
		static constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 1> skyboxInputLayoutDesc =
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

		success = gSkyboxPipeline.Create("SkyboxPipeline", { .SourceCodeFile = "SkyboxVertexShader.hlsl", .EntryPointName = "main", .ShaderMacros = nullptr },
			skyboxInputLayoutDesc.data(), static_cast<UINT>(skyboxInputLayoutDesc.size()), { .SourceCodeFile = "SkyboxPixelShader.hlsl", .EntryPointName = "main", .ShaderMacros = nullptr });
		if (!success) { return false; }

		// Lighting passes.
		static constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 4> lightingPassInputLayoutDesc =
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

		static constexpr std::array<D3D_SHADER_MACRO, 13> environmentLightingPassPixelShaderDefinitions =
		{
			D3D_SHADER_MACRO{.Name = "TEXTURE2D_SRV_TABLE_LENGTH", .Definition = RendererConstants::Texture2DSRVTableLengthString },
			D3D_SHADER_MACRO{.Name = "TEXTURE_CUBE_SRV_TABLE_LENGTH", .Definition = RendererConstants::TextureCubeSRVTableLengthString },
			D3D_SHADER_MACRO{.Name = "TEXTURE_SAMPLER_TABLE_LENGTH", .Definition = RendererConstants::TextureSamplerTableLengthString },
			D3D_SHADER_MACRO{.Name = "ENVIRONMENT_TEXTURE_CUBE_SRV_TABLE_INDEX", .Definition = RendererConstants::EnvironmentTextureSamplerTableIndexString },
			D3D_SHADER_MACRO{.Name = "ENVIRONMENT_TEXTURE_CUBE_SAMPLER_TABLE_INDEX", .Definition = RendererConstants::EnvironmentTextureCubeSRVTableIndexString },
			D3D_SHADER_MACRO{.Name = "COLOR_TEXTURE2D_SRV_TABLE_INDEX", .Definition = RendererConstants::ColorTexture2DSRVTableIndexString },
			D3D_SHADER_MACRO{.Name = "COLOR_TEXTURE_SAMPLER_TABLE_INDEX", .Definition = RendererConstants::ColorTextureSamplerTableIndexString },
			D3D_SHADER_MACRO{.Name = nullptr, .Definition = nullptr }
		};

		success = gEnvironmentLightPipeline.Create("EnvironmentLightPipeline", { .SourceCodeFile = "EnvironmentLightVertexShader.hlsl", .EntryPointName = "main", .ShaderMacros = nullptr },
			lightingPassInputLayoutDesc.data(), static_cast<UINT>(lightingPassInputLayoutDesc.size()),
			{ .SourceCodeFile = "EnvironmentLightPixelShader.hlsl", .EntryPointName = "main", .ShaderMacros = environmentLightingPassPixelShaderDefinitions.data() });
		if (!success) { return false; }

		static constexpr std::array<D3D_SHADER_MACRO, 13> lightingPassPixelShaderDefinitions =
		{
			D3D_SHADER_MACRO{.Name = "TEXTURE2D_SRV_TABLE_LENGTH", .Definition = RendererConstants::Texture2DSRVTableLengthString },
			D3D_SHADER_MACRO{.Name = "TEXTURE_SAMPLER_TABLE_LENGTH", .Definition = RendererConstants::TextureSamplerTableLengthString },
			D3D_SHADER_MACRO{.Name = "ENVIRONMENT_CUBEMAP_SRV_TABLE_INDEX", .Definition = RendererConstants::EnvironmentTextureCubeSRVTableIndexString },
			D3D_SHADER_MACRO{.Name = "COLOR_TEXTURE2D_SRV_TABLE_INDEX", .Definition = RendererConstants::ColorTexture2DSRVTableIndexString },
			D3D_SHADER_MACRO{.Name = "ROUGHNESS_TEXTURE2D_SRV_TABLE_INDEX", .Definition = RendererConstants::RoughnessTexture2DSRVTableIndexString },
			D3D_SHADER_MACRO{.Name = "METALLIC_TEXTURE2D_SRV_TABLE_INDEX", .Definition = RendererConstants::MetallicTexture2DSRVTableIndexString },
			D3D_SHADER_MACRO{.Name = "NORMAL_TEXTURE2D_SRV_TABLE_INDEX", .Definition = RendererConstants::NormalTexture2DSRVTableIndexString },
			D3D_SHADER_MACRO{.Name = "COLOR_TEXTURE_SAMPLER_TABLE_INDEX", .Definition = RendererConstants::ColorTextureSamplerTableIndexString },
			D3D_SHADER_MACRO{.Name = "ROUGHNESS_TEXTURE_SAMPLER_TABLE_INDEX", .Definition = RendererConstants::RoughnessTextureSamplerTableIndexString },
			D3D_SHADER_MACRO{.Name = "METALLIC_TEXTURE_SAMPLER_TABLE_INDEX", .Definition = RendererConstants::MetallicTextureSamplerTableIndexString },
			D3D_SHADER_MACRO{.Name = "NORMAL_TEXTURE_SAMPLER_TABLE_INDEX", .Definition = RendererConstants::NormalTextureSamplerTableIndexString },
			D3D_SHADER_MACRO{.Name = "PI", .Definition = LeviathanCore::MathLibrary::PiString },
			D3D_SHADER_MACRO{.Name = nullptr, .Definition = nullptr }
		};

		success = gDirectionalLightPipeline.Create("DirectionalLightPipeline",
			{ .SourceCodeFile = "DirectionalLightVertexShader.hlsl", .EntryPointName = "main", .ShaderMacros = nullptr },
			lightingPassInputLayoutDesc.data(),
			static_cast<UINT>(lightingPassInputLayoutDesc.size()),
			{ .SourceCodeFile = "DirectionalLightPixelShader.hlsl", .EntryPointName = "main", .ShaderMacros = lightingPassPixelShaderDefinitions.data() });
		if (!success) { return false; }

		success = gPointLightPipeline.Create("PointLightPipeline",
			{ .SourceCodeFile = "PointLightVertexShader.hlsl", .EntryPointName = "main", .ShaderMacros = nullptr },
			lightingPassInputLayoutDesc.data(),
			static_cast<UINT>(lightingPassInputLayoutDesc.size()),
			{ .SourceCodeFile = "PointLightPixelShader.hlsl", .EntryPointName = "main", .ShaderMacros = lightingPassPixelShaderDefinitions.data() });
		if (!success) { return false; }

		success = gSpotLightPipeline.Create("SpotLightPipeline",
			{ .SourceCodeFile = "SpotLightVertexShader.hlsl", .EntryPointName = "main", .ShaderMacros = nullptr },
			lightingPassInputLayoutDesc.data(),
			static_cast<UINT>(lightingPassInputLayoutDesc.size()),
			{ .SourceCodeFile = "SpotLightPixelShader.hlsl", .EntryPointName = "main", .ShaderMacros = lightingPassPixelShaderDefinitions.data() });
		if (!success) { return false; }

		// Post processing.
		static constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 2> postProcessPassInputLayoutDesc =
		{
			D3D11_INPUT_ELEMENT_DESC
			{
				.SemanticName = "POSITION",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = 0,
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

		success = gPostProcessPipeline.Create("PostProcessPipeline", { .SourceCodeFile = "PostProcessVertexShader.hlsl", .EntryPointName = "main", .ShaderMacros = nullptr },
			postProcessPassInputLayoutDesc.data(), static_cast<UINT>(postProcessPassInputLayoutDesc.size()),
			{ .SourceCodeFile = "PostProcessPixelShader.hlsl", .EntryPointName = "main", .ShaderMacros = nullptr });
		if (!success) { return false; }

		return success;
	}

	bool Renderer::InitializeRendererApi(unsigned int width, unsigned int height, void* windowPlatformHandle, bool vsync, unsigned int bufferCount)
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

		// Create screen render target view.
		success = CreateBackBufferRenderTargetView();
		if (!success) { return false; }

		// Create the depth/stencil buffer and view.
		success = CreateDepthStencilBufferAndView(width, height);
		if (!success) { return false; }

		// Create scene texture resource.
		success = CreateSceneTextureResource(width, height);
		if (!success) { return false; }

		// Create scene texture sampler state.
		D3D11_SAMPLER_DESC sceneTextureSamplerDesc = {};
		sceneTextureSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sceneTextureSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sceneTextureSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sceneTextureSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sceneTextureSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sceneTextureSamplerDesc.BorderColor[0] = 1.0f;
		sceneTextureSamplerDesc.BorderColor[1] = 0.0f;
		sceneTextureSamplerDesc.BorderColor[2] = 1.0f;
		sceneTextureSamplerDesc.BorderColor[3] = 1.0f;
		sceneTextureSamplerDesc.MipLODBias = 0.0f;
		sceneTextureSamplerDesc.MaxAnisotropy = 1;
		sceneTextureSamplerDesc.MinLOD = 0.0f;
		sceneTextureSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = gD3D11Device->CreateSamplerState(&sceneTextureSamplerDesc, &gSceneTextureSamplerState);
		if (FAILED(hr)) { return false; }

		// Create depth stencil states.
		D3D11_DEPTH_STENCIL_DESC gepthStencilStateWriteDepthDepthFuncLessStencilDisabledDesc = {};
		gepthStencilStateWriteDepthDepthFuncLessStencilDisabledDesc.DepthEnable = TRUE;
		gepthStencilStateWriteDepthDepthFuncLessStencilDisabledDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		gepthStencilStateWriteDepthDepthFuncLessStencilDisabledDesc.DepthFunc = D3D11_COMPARISON_LESS;
		gepthStencilStateWriteDepthDepthFuncLessStencilDisabledDesc.StencilEnable = FALSE;
		hr = gD3D11Device->CreateDepthStencilState(&gepthStencilStateWriteDepthDepthFuncLessStencilDisabledDesc, &gDepthStencilStateWriteDepthDepthFuncLessStencilDisabled);
		if (FAILED(hr)) { return false; };

		D3D11_DEPTH_STENCIL_DESC depthStencilStateNoWriteDepthDepthFuncEqualStencilDisabledDesc = {};
		depthStencilStateNoWriteDepthDepthFuncEqualStencilDisabledDesc.DepthEnable = TRUE;
		depthStencilStateNoWriteDepthDepthFuncEqualStencilDisabledDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilStateNoWriteDepthDepthFuncEqualStencilDisabledDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
		depthStencilStateNoWriteDepthDepthFuncEqualStencilDisabledDesc.StencilEnable = FALSE;
		hr = gD3D11Device->CreateDepthStencilState(&depthStencilStateNoWriteDepthDepthFuncEqualStencilDisabledDesc, &gDepthStencilStateNoWriteDepthDepthFuncEqualStencilDisabled);
		if (FAILED(hr)) { return false; };

		D3D11_DEPTH_STENCIL_DESC depthStencilStateNoWriteDepthDepthFuncLessStencilDisabledDesc = {};
		depthStencilStateNoWriteDepthDepthFuncLessStencilDisabledDesc.DepthEnable = TRUE;
		depthStencilStateNoWriteDepthDepthFuncLessStencilDisabledDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilStateNoWriteDepthDepthFuncLessStencilDisabledDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilStateNoWriteDepthDepthFuncLessStencilDisabledDesc.StencilEnable = FALSE;
		hr = gD3D11Device->CreateDepthStencilState(&depthStencilStateNoWriteDepthDepthFuncLessStencilDisabledDesc, &gDepthStencilStateNoWriteDepthDepthFuncLessStencilDisabled);
		if (FAILED(hr)) { return false; };

		D3D11_DEPTH_STENCIL_DESC depthStencilStateDepthStencilDisabledDesc = {};
		depthStencilStateDepthStencilDisabledDesc.DepthEnable = FALSE;
		depthStencilStateDepthStencilDisabledDesc.StencilEnable = FALSE;
		hr = gD3D11Device->CreateDepthStencilState(&depthStencilStateDepthStencilDisabledDesc, &gDepthStencilStateDepthStencilDisabled);
		if (FAILED(hr)) { return false; };

		// Create rasterizer states.
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

		// Create blend states.
		D3D11_BLEND_DESC additiveBlendDesc = {};

		D3D11_RENDER_TARGET_BLEND_DESC additiveRenderTargetBlendDesc = {};
		additiveRenderTargetBlendDesc.BlendEnable = true;
		additiveRenderTargetBlendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		additiveRenderTargetBlendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		additiveRenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		additiveRenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		additiveRenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		additiveRenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		additiveRenderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		additiveBlendDesc.RenderTarget[0] = additiveRenderTargetBlendDesc;

		hr = gD3D11Device->CreateBlendState(&additiveBlendDesc, &gBlendStateAdditive);
		if (FAILED(hr)) { return false; }

		// Set the viewport.
		SetViewport(width, height);

		// Initialize shader compilation.
		// For d3d12 renderer implementation.
		// Initialize shader compilation.
		//hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
		//if (FAILED(hr)) {return false;};

		//hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
		//if (FAILED(hr)) {return false;};

		// Create compiled shader cache directory if it does not exist.
		if (!LeviathanCore::Serialize::FileExists(CompiledShaderCacheDirectory))
		{
			LeviathanCore::Serialize::MakeDirectory(CompiledShaderCacheDirectory);
		}

		// Create pipelines.
		if (!CreatePipelines())
		{
			return false;
		}

		// Create constant buffers.
		success = CreateDefaultConstantBuffer<ConstantBufferTypes::EquirectangularToCubemapConstantBuffer>(&gEquirectangularToCubemapBuffer);
		if (!success) { return false; }
		success = CreateDefaultConstantBuffer<ConstantBufferTypes::SkyboxConstantBuffer>(&gSkyboxBuffer);
		if (!success) { return false; }
		success = CreateDefaultConstantBuffer<ConstantBufferTypes::DirectionalLightConstantBuffer>(&gDirectionalLightBuffer);
		if (!success) { return false; }
		success = CreateDefaultConstantBuffer<ConstantBufferTypes::PointLightConstantBuffer>(&gPointLightBuffer);
		if (!success) { return false; }
		success = CreateDefaultConstantBuffer<ConstantBufferTypes::SpotLightConstantBuffer>(&gSpotLightBuffer);
		if (!success) { return false; }
		success = CreateDefaultConstantBuffer<ConstantBufferTypes::ObjectConstantBuffer>(&gObjectBuffer);
		if (!success) { return false; }

		// Set primitive topology.
		gD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		return success;
	}

	bool Renderer::ShutdownRendererApi()
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

		gSceneTextureResource.Reset();
		gSceneTextureRenderTargetView.Reset();
		gSceneTextureShaderResourceView.Reset();
		gSceneTextureSamplerState.Reset();

		gDepthStencilStateWriteDepthDepthFuncLessStencilDisabled.Reset();
		gDepthStencilStateNoWriteDepthDepthFuncEqualStencilDisabled.Reset();
		gDepthStencilStateDepthStencilDisabled.Reset();
		gRasterizerState.Reset();
		gBlendStateAdditive.Reset();
		gViewport = {};

		gEquirectangularToCubemapPipeline.Destroy();
		gSkyboxPipeline.Destroy();
		gEnvironmentLightPipeline.Destroy();
		gDirectionalLightPipeline.Destroy();
		gPointLightPipeline.Destroy();
		gSpotLightPipeline.Destroy();
		gPostProcessPipeline.Destroy();

		gEquirectangularToCubemapBuffer.Reset();
		gSkyboxBuffer.Reset();
		gDirectionalLightBuffer.Reset();
		gPointLightBuffer.Reset();
		gSpotLightBuffer.Reset();
		gObjectBuffer.Reset();

		gVertexBuffers.clear();
		gIndexBuffers.clear();
		gShaderResourceViews.clear();
		gRenderTargetViews.clear();
		gSamplerStates.clear();

		return true;
	}

	bool Renderer::ResizeWindowResources(unsigned int width, unsigned int height)
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

		// Set the viewport.
		SetViewport(width, height);

		// Release current scene texture resources.
		gSceneTextureResource.Reset();

		// Recreate scene texture resource.
		success = CreateSceneTextureResource(width, height);
		if (!success) { return false; }

		return success;
	}

	bool Renderer::CreateVertexBuffer(const void* vertexData, unsigned int vertexCount, size_t singleVertexStrideBytes, RendererResourceId::IdType& outId)
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

	bool Renderer::CreateIndexBuffer(const unsigned int* indexData, unsigned int indexCount, RendererResourceId::IdType& outId)
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

	void Renderer::DestroyVertexBuffer(RendererResourceId::IdType& resourceID)
	{
		gVertexBuffers.erase(resourceID);
		resourceID = RendererResourceId::InvalidId;
	}

	void Renderer::DestroyIndexBuffer(RendererResourceId::IdType& resourceID)
	{
		gIndexBuffers.erase(resourceID);
		resourceID = RendererResourceId::InvalidId;
	}

	bool Renderer::CreateTexture2D(uint32_t width, uint32_t height, const void* data, uint32_t rowPitchBytes, bool sRGB, bool HDR, bool generateMips, RendererResourceId::IdType& outID)
	{
		HRESULT hr = {};

		// Create texture 2D resource.
		D3D11_TEXTURE2D_DESC texture2DDesc;
		texture2DDesc.Width = width;
		texture2DDesc.Height = height;
		texture2DDesc.ArraySize = 1;
		texture2DDesc.Format = ((HDR) ? (DXGI_FORMAT_R16G16B16A16_FLOAT) : ((sRGB) ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM));
		texture2DDesc.SampleDesc.Count = 1;
		texture2DDesc.SampleDesc.Quality = 0;
		texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2DDesc.CPUAccessFlags = 0;

		outID = RendererResourceId::GetAvailableId();
		gShaderResourceViews.emplace(outID, nullptr);

		if (generateMips)
		{
			const uint32_t numMipLevels = GetNumMipLevels(width, height);

			texture2DDesc.MipLevels = numMipLevels; // 0 generates a full mipmap chain. Values greater than 0 can be used to specify a specific number of mipmap images.
			texture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // Must flag to be bound as a render target and shader resource for mipmap generation.
			texture2DDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // Flag that this resource will have generated mipmaps.

			std::vector<D3D11_SUBRESOURCE_DATA> initDatas(numMipLevels, {});
			uint32_t mipRowPitchBytes = rowPitchBytes;
			for (uint32_t i = 0; i < numMipLevels; ++i)
			{
				initDatas[i].pSysMem = data;
				initDatas[i].SysMemPitch = static_cast<UINT>(mipRowPitchBytes);

				mipRowPitchBytes /= 2u;
			}

			Microsoft::WRL::ComPtr<ID3D11Texture2D> tex = nullptr;
			hr = gD3D11Device->CreateTexture2D(&texture2DDesc, initDatas.data(), &tex);
			if (FAILED(hr)) { return false; }

			// Create shader resource view of texture 2D resource.
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = texture2DDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = numMipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;

			hr = gD3D11Device->CreateShaderResourceView(tex.Get(), &srvDesc, gShaderResourceViews.at(outID).GetAddressOf());
			if (FAILED(hr)) { return false; }

			// Generate mipmap chain.
			gD3D11DeviceContext->GenerateMips(gShaderResourceViews.at(outID).Get());
		}
		else
		{
			texture2DDesc.MipLevels = 1;
			texture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texture2DDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA initData = {};
			initData.pSysMem = data;
			initData.SysMemPitch = static_cast<UINT>(rowPitchBytes);

			Microsoft::WRL::ComPtr<ID3D11Texture2D> tex = nullptr;
			hr = gD3D11Device->CreateTexture2D(&texture2DDesc, &initData, &tex);
			if (FAILED(hr)) { return false; }

			// Create shader resource view of texture 2D resource.
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = texture2DDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;

			hr = gD3D11Device->CreateShaderResourceView(tex.Get(), &srvDesc, gShaderResourceViews.at(outID).GetAddressOf());
			if (FAILED(hr)) { return false; }
		}

		return true;
	}

	void Renderer::DestroyTexture(RendererResourceId::IdType& resourceID)
	{
		gShaderResourceViews.erase(resourceID);
		resourceID = RendererResourceId::InvalidId;
	}

	bool Renderer::CreateSampler(TextureSamplerFilter filter, TextureSamplerBorderMode borderMode, const float* borderColor, const uint32_t anisotropy, RendererResourceId::IdType& outId)
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
		samplerDesc.MaxAnisotropy = static_cast<UINT>(anisotropy);
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		outId = RendererResourceId::GetAvailableId();
		gSamplerStates.emplace(outId, nullptr);

		return SUCCEEDED(gD3D11Device->CreateSamplerState(&samplerDesc, gSamplerStates.at(outId).GetAddressOf()));
	}

	void Renderer::DestroySampler(RendererResourceId::IdType& resourceID)
	{
		gSamplerStates.erase(resourceID);
		resourceID = RendererResourceId::InvalidId;
	}

	bool Renderer::CreateTextureCube(RendererResourceId::IdType& outId)
	{
		static const LeviathanRenderer::LinearColor textureColor(15, 15, 15, 0);

		// TODO: Take in texture data for each face
		D3D11_TEXTURE2D_DESC faceDesc = {};
		faceDesc.Width = 1;
		faceDesc.Height = 1;
		faceDesc.MipLevels = 1;
		faceDesc.ArraySize = 6;
		faceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		faceDesc.CPUAccessFlags = 0;
		faceDesc.SampleDesc.Count = 1;
		faceDesc.SampleDesc.Quality = 0;
		faceDesc.Usage = D3D11_USAGE_DEFAULT;
		faceDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		faceDesc.CPUAccessFlags = 0;
		faceDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		// Texture cube shader resource view description.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = faceDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = faceDesc.MipLevels;
		srvDesc.TextureCube.MostDetailedMip = 0;

		// Get texture data for each face of the cube texture.
		std::array<D3D11_SUBRESOURCE_DATA, 6> data = {};
		for (size_t cubemapFaceIndex = 0; cubemapFaceIndex < 6; ++cubemapFaceIndex)
		{
			// Pointer to the pixel data.
			data[cubemapFaceIndex].pSysMem = &textureColor;
			// Line width in bytes.
			data[cubemapFaceIndex].SysMemPitch = faceDesc.Width * 4; // Texture width X bytes per pixel.
			// Only used for 3D textures.
			data[cubemapFaceIndex].SysMemSlicePitch = 0;
		}

		// Create texture resource.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex = {};
		HRESULT hr = gD3D11Device->CreateTexture2D(&faceDesc, data.data(), &tex);
		if (FAILED(hr)) { return false; }

		// Create the shader resource view of the resource to use in shaders.
		outId = RendererResourceId::GetAvailableId();
		gShaderResourceViews.emplace(outId, nullptr);

		hr = gD3D11Device->CreateShaderResourceView(tex.Get(), &srvDesc, gShaderResourceViews.at(outId).GetAddressOf());
		if (FAILED(hr)) { return false; }

		return true;
	}

	void Renderer::DestroyTextureCube(RendererResourceId::IdType& resourceID)
	{
		gShaderResourceViews.erase(resourceID);
		resourceID = RendererResourceId::InvalidId;
	}

	bool Renderer::CreateTextureCubeRenderTarget(uint32_t width, uint32_t height, RendererResourceId::IdType** outRenderTargetIds, RendererResourceId::IdType& outShaderResourceId)
	{
		D3D11_TEXTURE2D_DESC faceDesc = {};
		faceDesc.Width = static_cast<UINT>(width);
		faceDesc.Height = static_cast<UINT>(height);
		faceDesc.MipLevels = 1;
		faceDesc.ArraySize = 6;
		faceDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		faceDesc.CPUAccessFlags = 0;
		faceDesc.SampleDesc.Count = 1;
		faceDesc.SampleDesc.Quality = 0;
		faceDesc.Usage = D3D11_USAGE_DEFAULT;
		faceDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		faceDesc.CPUAccessFlags = 0;
		faceDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		// Texture cube render target view description.
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = faceDesc.Format;
		rtvDesc.Texture2DArray.ArraySize = 1;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;

		// Texture cube shader resource view description.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = faceDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = faceDesc.MipLevels;
		srvDesc.TextureCube.MostDetailedMip = 0;

		// Create texture resource.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex = {};
		HRESULT hr = gD3D11Device->CreateTexture2D(&faceDesc, nullptr, &tex);
		if (FAILED(hr)) { return false; }

		// Create the render target view of each face of the cubemap to render to.
		for (UINT i = 0; i < 6; ++i)
		{
			RendererResourceId::IdType rtId = RendererResourceId::GetAvailableId();
			(*outRenderTargetIds)[i] = rtId;
			gRenderTargetViews.emplace(rtId, nullptr);
			rtvDesc.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, i, 1);
			hr = gD3D11Device->CreateRenderTargetView(tex.Get(), &rtvDesc, gRenderTargetViews.at(rtId).GetAddressOf());
			if (FAILED(hr)) { return false; }
		}

		// Create the shader resource view of the resource to use in shaders.
		outShaderResourceId = RendererResourceId::GetAvailableId();
		gShaderResourceViews.emplace(outShaderResourceId, nullptr);

		hr = gD3D11Device->CreateShaderResourceView(tex.Get(), &srvDesc, gShaderResourceViews.at(outShaderResourceId).GetAddressOf());
		if (FAILED(hr)) { return false; }

		return true;
	}

	void Renderer::DestroyTextureCubeRenderTarget(RendererResourceId::IdType* renderTargetIds, RendererResourceId::IdType& shaderResourceId)
	{
		for (size_t i = 0; i < 6; ++i)
		{
			gRenderTargetViews.erase(renderTargetIds[i]);
		}
		gShaderResourceViews.erase(shaderResourceId);
	}

	void Renderer::SetEquirectangularToCubemapPipeline(RendererResourceId::IdType HDRTexture2DResourceId, RendererResourceId::IdType HDRTextureSamplerId)
	{
		gD3D11DeviceContext->IASetInputLayout(gEquirectangularToCubemapPipeline.GetInputLayout());
		gD3D11DeviceContext->VSSetShader(gEquirectangularToCubemapPipeline.GetVertexShader(), nullptr, 0);
		gD3D11DeviceContext->PSSetShader(gEquirectangularToCubemapPipeline.GetPixelShader(), nullptr, 0);
		gD3D11DeviceContext->VSSetConstantBuffers(0, 1, gEquirectangularToCubemapBuffer.GetAddressOf());
		gD3D11DeviceContext->PSSetShaderResources(0, 1, gShaderResourceViews.at(HDRTexture2DResourceId).GetAddressOf());
		gD3D11DeviceContext->PSSetSamplers(0, 1, gSamplerStates.at(HDRTextureSamplerId).GetAddressOf());
	}

	bool Renderer::UpdateEquirectangularToCubemapBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return UpdateConstantBuffer(gEquirectangularToCubemapBuffer.Get(), byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	void Renderer::SetSkyboxPipeline(RendererResourceId::IdType skyboxTextureCubeId, RendererResourceId::IdType skyboxTextureCubeSamplerId)
	{
		gD3D11DeviceContext->IASetInputLayout(gSkyboxPipeline.GetInputLayout());
		gD3D11DeviceContext->VSSetShader(gSkyboxPipeline.GetVertexShader(), nullptr, 0);
		gD3D11DeviceContext->PSSetShader(gSkyboxPipeline.GetPixelShader(), nullptr, 0);
		gD3D11DeviceContext->VSSetConstantBuffers(0, 1, gSkyboxBuffer.GetAddressOf());
		gD3D11DeviceContext->PSSetShaderResources(0, 1, gShaderResourceViews.at(skyboxTextureCubeId).GetAddressOf());
		gD3D11DeviceContext->PSSetSamplers(0, 1, gSamplerStates.at(skyboxTextureCubeSamplerId).GetAddressOf());
	}

	bool Renderer::UpdateSkyboxBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return UpdateConstantBuffer(gSkyboxBuffer.Get(), byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	void Renderer::ClearScreenRenderTarget(const float* clearColor)
	{
		gD3D11DeviceContext->ClearRenderTargetView(gBackBufferRenderTargetView.Get(), clearColor);
	}

	void Renderer::ClearSceneRenderTarget(const float* clearColor)
	{
		gD3D11DeviceContext->ClearRenderTargetView(gSceneTextureRenderTargetView.Get(), clearColor);
	}

	void Renderer::ClearRenderTarget(RendererResourceId::IdType renderTarget, const float* clearColor)
	{
		gD3D11DeviceContext->ClearRenderTargetView(gRenderTargetViews.at(renderTarget).Get(), clearColor);
	}

	void Renderer::ClearDepthStencil(float clearDepth, unsigned char clearStencil)
	{
		gD3D11DeviceContext->ClearDepthStencilView(gDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
	}

	void Renderer::SetScreenRenderTarget()
	{
		gD3D11DeviceContext->OMSetRenderTargets(1, gBackBufferRenderTargetView.GetAddressOf(), nullptr);
	}

	void Renderer::SetSceneRenderTarget()
	{
		gD3D11DeviceContext->OMSetRenderTargets(1, gSceneTextureRenderTargetView.GetAddressOf(), gDepthStencilView.Get());
	}

	void Renderer::SetRenderTarget(const RendererResourceId::IdType renderTargetId)
	{
		gD3D11DeviceContext->OMSetRenderTargets(1, gRenderTargetViews.at(renderTargetId).GetAddressOf(), nullptr);
	}

	void Renderer::SetAmbientLightPipeline()
	{
		gD3D11DeviceContext->IASetInputLayout(gDirectionalLightPipeline.GetInputLayout());
		gD3D11DeviceContext->VSSetShader(gEnvironmentLightPipeline.GetVertexShader(), nullptr, 0);
		gD3D11DeviceContext->PSSetShader(gEnvironmentLightPipeline.GetPixelShader(), nullptr, 0);
		gD3D11DeviceContext->VSSetConstantBuffers(0, 1, gObjectBuffer.GetAddressOf());
		gD3D11DeviceContext->PSSetShaderResources(0, RendererConstants::Texture2DSRVTableLength, gTexture2DSRVTable[0].GetAddressOf());
		gD3D11DeviceContext->PSSetShaderResources(1, RendererConstants::TextureCubeSRVTableLength, gTextureCubeSRVTable[0].GetAddressOf());
		gD3D11DeviceContext->PSSetSamplers(0, RendererConstants::TextureSamplerTableLength, gTextureSamplerTable[0].GetAddressOf());
	}

	void Renderer::UnbindShaderResources()
	{
		ID3D11ShaderResourceView* nullShaderResourceView = nullptr;
		gD3D11DeviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);
	}

	void Renderer::SetDirectionalLightPipeline()
	{
		gD3D11DeviceContext->IASetInputLayout(gDirectionalLightPipeline.GetInputLayout());
		gD3D11DeviceContext->VSSetShader(gDirectionalLightPipeline.GetVertexShader(), nullptr, 0);
		gD3D11DeviceContext->PSSetShader(gDirectionalLightPipeline.GetPixelShader(), nullptr, 0);
		gD3D11DeviceContext->VSSetConstantBuffers(0, 1, gObjectBuffer.GetAddressOf());
		gD3D11DeviceContext->VSSetConstantBuffers(1, 1, gDirectionalLightBuffer.GetAddressOf());
		gD3D11DeviceContext->PSSetConstantBuffers(0, 1, gDirectionalLightBuffer.GetAddressOf());
		gD3D11DeviceContext->PSSetShaderResources(0, RendererConstants::Texture2DSRVTableLength, gTexture2DSRVTable[0].GetAddressOf());
		gD3D11DeviceContext->PSSetSamplers(0, RendererConstants::TextureSamplerTableLength, gTextureSamplerTable[0].GetAddressOf());
	}

	void Renderer::SetPointLightPipeline()
	{
		gD3D11DeviceContext->IASetInputLayout(gPointLightPipeline.GetInputLayout());
		gD3D11DeviceContext->VSSetShader(gPointLightPipeline.GetVertexShader(), nullptr, 0);
		gD3D11DeviceContext->PSSetShader(gPointLightPipeline.GetPixelShader(), nullptr, 0);
		gD3D11DeviceContext->VSSetConstantBuffers(0, 1, gObjectBuffer.GetAddressOf());
		gD3D11DeviceContext->VSSetConstantBuffers(1, 1, gPointLightBuffer.GetAddressOf());
		gD3D11DeviceContext->PSSetConstantBuffers(0, 1, gPointLightBuffer.GetAddressOf());
		gD3D11DeviceContext->PSSetShaderResources(0, RendererConstants::Texture2DSRVTableLength, gTexture2DSRVTable[0].GetAddressOf());
		gD3D11DeviceContext->PSSetSamplers(0, RendererConstants::TextureSamplerTableLength, gTextureSamplerTable[0].GetAddressOf());
	}

	void Renderer::SetSpotLightPipeline()
	{
		gD3D11DeviceContext->IASetInputLayout(gSpotLightPipeline.GetInputLayout());
		gD3D11DeviceContext->VSSetShader(gSpotLightPipeline.GetVertexShader(), nullptr, 0);
		gD3D11DeviceContext->PSSetShader(gSpotLightPipeline.GetPixelShader(), nullptr, 0);
		gD3D11DeviceContext->VSSetConstantBuffers(0, 1, gObjectBuffer.GetAddressOf());
		gD3D11DeviceContext->VSSetConstantBuffers(1, 1, gSpotLightBuffer.GetAddressOf());
		gD3D11DeviceContext->PSSetConstantBuffers(0, 1, gSpotLightBuffer.GetAddressOf());
		gD3D11DeviceContext->PSSetShaderResources(0, RendererConstants::Texture2DSRVTableLength, gTexture2DSRVTable[0].GetAddressOf());
		gD3D11DeviceContext->PSSetSamplers(0, RendererConstants::TextureSamplerTableLength, gTextureSamplerTable[0].GetAddressOf());
	}

	void Renderer::SetPostProcessPipeline()
	{
		gD3D11DeviceContext->IASetInputLayout(gPostProcessPipeline.GetInputLayout());
		gD3D11DeviceContext->VSSetShader(gPostProcessPipeline.GetVertexShader(), nullptr, 0);
		gD3D11DeviceContext->PSSetShader(gPostProcessPipeline.GetPixelShader(), nullptr, 0);
		gD3D11DeviceContext->PSSetShaderResources(0, 1, gSceneTextureShaderResourceView.GetAddressOf());
		gD3D11DeviceContext->PSSetSamplers(0, 1, gSceneTextureSamplerState.GetAddressOf());
	}

	void Renderer::Present()
	{
		gSwapChain->Present(((gVSync) ? 1 : 0), 0);
	}

	void Renderer::DrawIndexed(const unsigned int indexCount, size_t singleVertexStrideBytes, const RendererResourceId::IdType vertexBufferId, const RendererResourceId::IdType indexBufferId)
	{
		UINT stride = static_cast<UINT>(singleVertexStrideBytes);
		UINT offset = 0;
		gD3D11DeviceContext->IASetVertexBuffers(0, 1, gVertexBuffers.at(static_cast<size_t>(vertexBufferId)).GetAddressOf(), &stride, &offset);
		gD3D11DeviceContext->IASetIndexBuffer(gIndexBuffers.at(static_cast<size_t>(indexBufferId)).Get(), DXGI_FORMAT_R32_UINT, 0);

		gD3D11DeviceContext->DrawIndexed(indexCount, 0, 0);
	}

	bool Renderer::UpdateObjectBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return UpdateConstantBuffer(gObjectBuffer.Get(), byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	bool Renderer::UpdateDirectionalLightBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return UpdateConstantBuffer(gDirectionalLightBuffer.Get(), byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	bool Renderer::UpdatePointLightBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return UpdateConstantBuffer(gPointLightBuffer.Get(), byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	bool Renderer::UpdateSpotLightBufferData(size_t byteOffsetIntoBuffer, const void* pNewData, size_t byteWidth)
	{
		return UpdateConstantBuffer(gSpotLightBuffer.Get(), byteOffsetIntoBuffer, pNewData, byteWidth);
	}

	void Renderer::SetDepthStencilStateWriteDepthDepthFuncLessStencilDisabled()
	{
		gD3D11DeviceContext->OMSetDepthStencilState(gDepthStencilStateWriteDepthDepthFuncLessStencilDisabled.Get(), 0);
	}

	void Renderer::SetDepthStencilStateDepthStencilDisabled()
	{
		gD3D11DeviceContext->OMSetDepthStencilState(gDepthStencilStateDepthStencilDisabled.Get(), 0);
	}

	void Renderer::SetDepthStencilStateNoWriteDepthDepthFuncEqualStencilDisabled()
	{
		gD3D11DeviceContext->OMSetDepthStencilState(gDepthStencilStateNoWriteDepthDepthFuncEqualStencilDisabled.Get(), 0);
	}

	void Renderer::SetDepthStencilStateNoWriteDepthDepthFuncLessStencilDisabled()
	{
		gD3D11DeviceContext->OMSetDepthStencilState(gDepthStencilStateNoWriteDepthDepthFuncLessStencilDisabled.Get(), 0);
	}

	void Renderer::SetBlendStateAdditive()
	{
		gD3D11DeviceContext->OMSetBlendState(gBlendStateAdditive.Get(), nullptr, 0xffffffff);
	}

	void Renderer::SetBlendStateBlendDisabled()
	{
		gD3D11DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	}

	void Renderer::SetViewport(uint32_t width, uint32_t height)
	{
		gViewport.Width = static_cast<float>(width);
		gViewport.Height = static_cast<float>(height);
		gViewport.TopLeftX = 0.0f;
		gViewport.TopLeftY = 0.0f;
		gViewport.MinDepth = 0.0f;
		gViewport.MaxDepth = 1.0f;
		gD3D11DeviceContext->RSSetViewports(1, &gViewport);
	}

	void Renderer::SetEnvironmentTextureCubeResource(RendererResourceId::IdType textureCubeId)
	{
		gTextureCubeSRVTable[RendererConstants::EnvironmentTextureCubeSRVTableIndex] = gShaderResourceViews.at(textureCubeId);
	}

	void Renderer::SetColorTexture2DResource(RendererResourceId::IdType texture2DId)
	{
		gTexture2DSRVTable[RendererConstants::ColorTexture2DSRVTableIndex] = gShaderResourceViews.at(texture2DId);
	}

	void Renderer::SetRoughnessTexture2DResource(RendererResourceId::IdType texture2DId)
	{
		gTexture2DSRVTable[RendererConstants::RoughnessTexture2DSRVTableIndex] = gShaderResourceViews.at(texture2DId);
	}

	void Renderer::SetMetallicTexture2DResource(RendererResourceId::IdType texture2DId)
	{
		gTexture2DSRVTable[RendererConstants::MetallicTexture2DSRVTableIndex] = gShaderResourceViews.at(texture2DId);
	}

	void Renderer::SetNormalTexture2DResource(RendererResourceId::IdType texture2DId)
	{
		gTexture2DSRVTable[RendererConstants::NormalTexture2DSRVTableIndex] = gShaderResourceViews.at(texture2DId);
	}

	void Renderer::SetEnvironmentTextureSampler(RendererResourceId::IdType samplerId)
	{
		gTextureSamplerTable[RendererConstants::EnvironmentTextureSamplerTableIndex] = gSamplerStates.at(samplerId);
	}

	void Renderer::SetColorTextureSampler(RendererResourceId::IdType samplerId)
	{
		gTextureSamplerTable[RendererConstants::ColorTextureSamplerTableIndex] = gSamplerStates.at(samplerId);
	}

	void Renderer::SetRoughnessTextureSampler(RendererResourceId::IdType samplerId)
	{
		gTextureSamplerTable[RendererConstants::RoughnessTextureSamplerTableIndex] = gSamplerStates.at(samplerId);
	}

	void Renderer::SetMetallicTextureSampler(RendererResourceId::IdType samplerId)
	{
		gTextureSamplerTable[RendererConstants::MetallicTextureSamplerTableIndex] = gSamplerStates.at(samplerId);
	}

	void Renderer::SetNormalTextureSampler(RendererResourceId::IdType samplerId)
	{
		gTextureSamplerTable[RendererConstants::NormalTextureSamplerTableIndex] = gSamplerStates.at(samplerId);
	}

#ifdef LEVIATHAN_WITH_TOOLS
	bool Renderer::ImGuiRendererInitialize()
	{
		return ImGui_ImplDX11_Init(gD3D11Device.Get(), gD3D11DeviceContext.Get());
	}

	void Renderer::ImGuiRendererShutdown()
	{
		ImGui_ImplDX11_Shutdown();
	}

	void Renderer::ImGuiRendererNewFrame()
	{
		ImGui_ImplDX11_NewFrame();
	}

	void Renderer::ImGuiRender()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
#endif // LEVIATHAN_WITH_TOOLS.
}