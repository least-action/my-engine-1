#pragma once
#include "MainWindow.h"
#include "CubeBox.h"
#include <windows.h>
#include <d3d11.h>
#include <directxmath.h>


#include <memory>

class App {
	struct ConstantBuffer
	{
		DirectX::XMMATRIX mWorld;
		DirectX::XMMATRIX mView;
		DirectX::XMMATRIX mProjection;
		DirectX::XMFLOAT4 vLightDir[2];
		DirectX::XMFLOAT4 vLightColor[2];
		DirectX::XMFLOAT4 vOutputColor;
	};

	HRESULT InitWindow();
	HRESULT InitD3D();
	void CleanupDevice();
	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
	void Render();
    
	std::unique_ptr<MainWindow> mMainWindow;

	IDXGISwapChain* mSwapChain = nullptr;
	ID3D11Device* mDevice = nullptr;
	ID3D11DeviceContext* mContext = nullptr;
	ID3D11RenderTargetView* mRenderTargetView = nullptr;
	ID3D11Texture2D* mDepthStencil = nullptr;
	ID3D11DepthStencilView* mDepthStencilView = nullptr;
	ID3D11Buffer* mConstantBuffer = nullptr;
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;

	CubeBox cubeBox;
		
public:
	App() : mMainWindow(new MainWindow()) {};

	int Initialize();
	int Run();
};
