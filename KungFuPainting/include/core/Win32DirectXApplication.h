#ifndef _PSYKO_WIN32DIRECTXAPPLICATION
#define _PSYKO_WIN32DIRECTXAPPLICATION

#include <string>
#include <Windows.h>
#include <d3d11.h>

namespace psyko 
{
	class Win32DirectXApplication
	{
	public:
		Win32DirectXApplication();
		~Win32DirectXApplication();
		
		int Init(HINSTANCE hInstance, PWSTR pCmdLine, int nCmdShow);
		virtual int SetUp() { return 0; }
		int Run();
		
		void SetWindowTitle(std::wstring value);
		void SetViewport(float x, float y, float width, float height);


		// return true if message was processed, false otherwise
		virtual bool HandleWin32Message(UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
		IDXGISwapChain* swapChain;
		ID3D11Texture2D* backBuffer;
		ID3D11RenderTargetView* backBufferView;
		ID3D11Texture2D* depthStencilBuffer;
		ID3D11DepthStencilView* depthStencilView;
		unsigned int windowWidth;
		unsigned int windowHeight;

		virtual void OnResize(unsigned int width, unsigned int height);
		virtual int ProcessApplication() { return 0; };

	private:
		HINSTANCE hInstance;
		HWND hWnd;
		HDC hDC;
		std::wstring windowTitle;
		unsigned int msaaAmount;
		unsigned int msaaQualityLevels;
		D3D_FEATURE_LEVEL featureLevel;

		

		void InitWindow(PWSTR pCmdLine, int nCmdShow);
		void InitDirect3D();
		void CreateDeviceAndContext();
		void InitMSAA();
		void InitSwapChain();
		void InitRenderTargetView();
		void InitDepthAndStencil();
		DXGI_SWAP_CHAIN_DESC CreateSwapChainDescriptor();
		D3D11_TEXTURE2D_DESC CreateDepthStencilTextureDescriptor();
	};

}
#endif