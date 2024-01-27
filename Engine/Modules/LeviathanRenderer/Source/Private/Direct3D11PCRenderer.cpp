#include "Renderer.h"

namespace LeviathanRenderer
{
	namespace Renderer
	{
		// Device, device context and swap chain.
		static Microsoft::WRL::ComPtr<ID3D11Device> D3D11Device = {};
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext> D3D11DeviceContext = {};
		static Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain = {};
		static D3D_FEATURE_LEVEL FeatureLevel = {};

		// Render target view for the swap chain back buffer.
		static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView = {};

		// Depth/stencil target view for the swap chain depth/stencil buffer.
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView = {};

		// Texture resource to associate to the depth stencil view.
		static Microsoft::WRL::ComPtr<ID3D11Resource> DepthStencilBuffer = {};

		// Define the functionality of the depth/stencil stage.
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState = {};

		// Define the functionality of the rasterizer stage.
		static Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState = {};
		static D3D11_VIEWPORT Viewport = {};

		bool InitializeRendererApi(unsigned int width, unsigned int height, unsigned int verticalRefreshRate, void* windowPlatformHandle, bool vsync, unsigned int bufferCount)
		{
			// Create device and swap chain.
			DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
			swapChainDesc.BufferCount = bufferCount;
			swapChainDesc.BufferDesc.Width = width;
			swapChainDesc.BufferDesc.Height = height;
			swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.BufferDesc.RefreshRate.Numerator = ((vsync) ? verticalRefreshRate : 0);
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.OutputWindow = static_cast<HWND>(windowPlatformHandle);
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.Windowed = TRUE;

			UINT createDeviceFlags = 0;
#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
			createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif //LEVIATHAN_BUILD_CONFIG_DEBUG

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

			HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				NULL,
				createDeviceFlags,
				featureLevels.data(), static_cast<unsigned int>(featureLevels.size()),
				D3D11_SDK_VERSION,
				&swapChainDesc,
				&SwapChain, &D3D11Device, &FeatureLevel, &D3D11DeviceContext);

			if (FAILED(hr))
			{
				return false;
			}

			// Create render target view.


			return true;
		}
	}
}