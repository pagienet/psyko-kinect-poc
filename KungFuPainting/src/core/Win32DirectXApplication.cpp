#include "core/Win32DirectXApplication.h"

#include <iostream>
#include "error/errorcodes.h"
#include "utils/PrecisionTimer.h"

namespace psyko
{
	
	// some boiler plate to handle the application message loop and let the instance handle it instead
	Win32DirectXApplication* GetApplicationForWindow(HWND hWnd) 
	{
		LONG_PTR userData = GetWindowLongPtr(hWnd, GWLP_USERDATA);
		return reinterpret_cast<Win32DirectXApplication*>(userData);
	}

	void RegisterCreateParamsAsUserData(HWND hWnd, LPARAM lParam) 
	{
		// map app to hWnd in user data
		CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		Win32DirectXApplication* app = reinterpret_cast<Win32DirectXApplication*>(createStruct->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)app);
	}

	bool ForwardMessageToWindow(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
	{
		Win32DirectXApplication* application = GetApplicationForWindow(hWnd);
		if (application)
			return application->HandleWin32Message(uMsg, wParam, lParam);
		else 
			return false;
	}

	LRESULT CALLBACK HandleWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg) {
		case WM_CREATE:
			RegisterCreateParamsAsUserData(hWnd, lParam);
			break;
		default:
			if (ForwardMessageToWindow(hWnd, uMsg, wParam, lParam))
				return 0;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	unsigned int TestAndGetMSAAQualityLevels(ID3D11Device* device, int msaaAmount)
	{
		UINT numQualityLevels;
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		device->CheckMultisampleQualityLevels(format, msaaAmount, &numQualityLevels);
		return numQualityLevels;
	}


	Win32DirectXApplication::Win32DirectXApplication()
		: 
		device(0), deviceContext(0), swapChain(0), 
		backBuffer(0), backBufferView(0), 
		depthStencilBuffer(0), depthStencilView(0),
		hInstance(0), hWnd(0), hDC(0), windowTitle(L"Psyko App"), windowWidth(800), windowHeight(600), msaaAmount(4), msaaQualityLevels(0)
	{
	}

	Win32DirectXApplication::~Win32DirectXApplication()
	{
		UnregisterClass(L"PsykoWindow", hInstance);
	}

	int Win32DirectXApplication::Init(HINSTANCE hInstance, PWSTR pCmdLine, int nCmdShow)
	{
		this->hInstance = hInstance;
		int code = InitWindow(pCmdLine, nCmdShow);
		if (code) return code;
		code = InitDirect3D();
		if (code) return code;
		return SetUp();
	}

	void Win32DirectXApplication::SetViewport(float x, float y, float width, float height)
	{
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = x;
		viewport.TopLeftY = y;
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;		
		deviceContext->RSSetViewports(1, &viewport);
	}


	void Win32DirectXApplication::SetWindowTitle(std::wstring value)
	{
		windowTitle = value;
		if (hWnd)
			SetWindowText(hWnd, windowTitle.c_str());
	}

	int Win32DirectXApplication::Run()
	{
		const double aimTime = 16.6666667;

		MSG message = { };

		while (true) {
			if (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
				TranslateMessage(&message);
				DispatchMessage(&message);
				if (message.message == WM_QUIT) return 0;
			}
			else {
				double time = GetGlobalTimer();
				int errorCode = ProcessApplication();
				if (errorCode) return errorCode;
				
				double currentFrameTime = GetGlobalTimer() - time;
				
				// aim for 60 fps
				Sleep(int((int(currentFrameTime/aimTime) + 1)*aimTime - currentFrameTime));
			}
		}
	}

	int Win32DirectXApplication::InitWindow(PWSTR pCmdLine, int nCmdShow) 
	{
		const wchar_t CLASS_NAME[] = L"PsykoWindow";
		
		WNDCLASS wndClass = {};

		wndClass.lpfnWndProc = HandleWindowProc;
		wndClass.hInstance = hInstance;
		wndClass.lpszClassName = CLASS_NAME;
		wndClass.hCursor = LoadCursorW(NULL, IDC_ARROW);

		// TODO: exit if returns 0 (clean return or error code?)
		RegisterClass(&wndClass);
	
		hWnd = CreateWindowEx(
			WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,                     
			CLASS_NAME,            
			windowTitle.c_str(),			
			WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,

			NULL,
			NULL,
			hInstance,
			this	// to be passed on in WM_CREATE so we can store pointer
		);

		if (!hWnd)
			return WINDOW_CREATION_FAILED;

		ShowWindow(hWnd, nCmdShow);
		return 0;
	}

	int Win32DirectXApplication::InitDirect3D()
	{
		int code = CreateDeviceAndContext();
		if (code) return code;
		InitMSAA();
		code = InitSwapChain();		
		if (code) return code;
		SetViewport(0, 0, (float)windowWidth, (float)windowHeight);
		return 0;
	}

	void Win32DirectXApplication::InitMSAA()
	{
		msaaQualityLevels = TestAndGetMSAAQualityLevels(device, msaaAmount);
		if (msaaQualityLevels == 0) {
			msaaAmount = 1;
			msaaQualityLevels = TestAndGetMSAAQualityLevels(device, msaaAmount);
		}
	}

	bool Win32DirectXApplication::HandleWin32Message(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg) { 
			case WM_CLOSE:
				DestroyWindow(hWnd);
				return true;
			case WM_DESTROY:
				PostQuitMessage(0);
				return true;		
			case WM_SIZE:
				OnResize(LOWORD(lParam), HIWORD(lParam));
				return true;
		}
		return false;
	}

	void Win32DirectXApplication::OnResize(unsigned int width, unsigned int height)
	{
		this->windowWidth = width;
		this->windowHeight = height;
		// todo: resize backbuffer, then we can set viewport?
		//if (deviceContext) 
			//SetViewport(0, 0, (float)windowWidth, (float)windowHeight);
	}

	int Win32DirectXApplication::CreateDeviceAndContext()
	{				
		// todo: provide settings with driver type
		// todo: provide different feature levels
		UINT createDeviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		const int numFeatureLevels = 1;
		D3D_FEATURE_LEVEL requestedFeatureLevels[] = { D3D_FEATURE_LEVEL_10_1 };
		HRESULT result = D3D11CreateDevice(0,  D3D_DRIVER_TYPE_HARDWARE,  0, createDeviceFlags, requestedFeatureLevels, sizeof(requestedFeatureLevels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &device, &featureLevel, &deviceContext);

		if (FAILED(result))
			return D3D_DEVICE_CONTEXT_CREATION_FAILED;

		bool found = false;
		for (int i = 0; i < numFeatureLevels && !found; ++i)
			found = featureLevel == requestedFeatureLevels[i];

		if (!found)
			return D3D_VERSION_NOT_SUPPORTED;

		return 0;
	}
	
	int Win32DirectXApplication::InitSwapChain()
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc = CreateSwapChainDescriptor();

		// bunch of COM stuff to get the factory with which the device was created
		IDXGIDevice* dxgiDevice = 0;
		IDXGIAdapter* dxgiAdapter = 0;
		IDXGIFactory* dxgiFactory = 0;
		HRESULT result;

		result = device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
		if (FAILED(result))
			return D3D_QUERY_INTERFACE_FAILED;
		
		result = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
		if (FAILED(result)) {
			dxgiDevice->Release();
			return D3D_QUERY_INTERFACE_FAILED;
		}
		
		result = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
		if (FAILED(result)) {
			dxgiDevice->Release();
			dxgiAdapter->Release();
			return D3D_QUERY_INTERFACE_FAILED;
		}
		
		result = dxgiFactory->CreateSwapChain(device, &swapChainDesc, &swapChain);
		dxgiDevice->Release();
		dxgiAdapter->Release();
		dxgiFactory->Release();
		
		if (FAILED(result))
			return D3D_SWAP_CHAIN_CREATION_FAILED;

		int code = InitRenderTargetView();
		if (code) return code;
		code = InitDepthAndStencil();
		if (code) return code;
		deviceContext->OMSetRenderTargets(1, &backBufferView, depthStencilView);

		return 0;
	}

	int Win32DirectXApplication::InitRenderTargetView()
	{
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&backBuffer));
		HRESULT result = device->CreateRenderTargetView(backBuffer, 0, &backBufferView);
		backBuffer->Release();
		if (FAILED(result)) return D3D_BACKBUFFER_CREATION_FAILED;
		return 0;
	}

	int Win32DirectXApplication::InitDepthAndStencil()
	{
		D3D11_TEXTURE2D_DESC textureDescriptor = CreateDepthStencilTextureDescriptor();
		HRESULT result;

		result = device->CreateTexture2D(&textureDescriptor, 0, &depthStencilBuffer);
		if (FAILED(result)) return D3D_DEPTH_STENCIL_BUFFER_CREATION_FAILED;

		result = device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView);
		if (FAILED(result)) return D3D_DEPTH_STENCIL_BUFFER_CREATION_FAILED;
		return 0;
	}

	DXGI_SWAP_CHAIN_DESC Win32DirectXApplication::CreateSwapChainDescriptor()
	{
		DXGI_SWAP_CHAIN_DESC descriptor = { 0 };
		descriptor.BufferDesc.Width = windowWidth;
		descriptor.BufferDesc.Height = windowHeight;
		descriptor.BufferDesc.RefreshRate.Numerator = 60;	// todo: provide way to set refresh rate
		descriptor.BufferDesc.RefreshRate.Denominator = 1;
		descriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		descriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		descriptor.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		descriptor.SampleDesc.Count = msaaAmount;
		descriptor.SampleDesc.Quality = msaaQualityLevels-1;	// todo: provide way to set quality
		descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		descriptor.BufferCount = 1;
		descriptor.OutputWindow = hWnd;
		descriptor.Windowed = true;
		descriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		// Todo: set flags for fullscreen mode change
		return descriptor;
	}

	D3D11_TEXTURE2D_DESC Win32DirectXApplication::CreateDepthStencilTextureDescriptor()
	{
		D3D11_TEXTURE2D_DESC descriptor = { 0 };
		descriptor.Width = windowWidth;
		descriptor.Height = windowHeight;
		descriptor.MipLevels = 1;
		descriptor.ArraySize = 1;
		descriptor.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descriptor.SampleDesc.Count = 4;
		descriptor.SampleDesc.Quality = msaaQualityLevels-1;	// todo: provide way to set quality
		descriptor.Usage = D3D11_USAGE_DEFAULT;
		descriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		return descriptor;
	}
}