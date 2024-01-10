#pragma once
#include "MainWindow.h"
#include "CubeBox.h"
#include "GroundSurface.h"
#include <windows.h>
#include <d3d11.h>
#include <directxmath.h>


#include <memory>

class App {
	struct ConstantBuffer
	{
		DirectX::XMMATRIX mView;
		DirectX::XMMATRIX mProjection;
		DirectX::XMFLOAT4 vLightDir[2];
		DirectX::XMFLOAT4 vLightColor[2];
		DirectX::XMFLOAT4 vOutputColor;
	};

	struct WorldContantBuffer
	{
		DirectX::XMMATRIX mWorld;
	};

	struct Eye
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 Look;
		DirectX::XMFLOAT3 Up;
		DirectX::XMFLOAT3 Right;
	};

	HRESULT InitWindow();
	HRESULT InitD3D();
	void CleanupDevice();
	void Render();

	void UpdateModels();

	std::unique_ptr<MainWindow> mMainWindow;

	IDXGISwapChain* mSwapChain = nullptr;
	ID3D11Device* mDevice = nullptr;
	ID3D11DeviceContext* mContext = nullptr;
	ID3D11RenderTargetView* mRenderTargetView = nullptr;
	ID3D11Texture2D* mDepthStencil = nullptr;
	ID3D11DepthStencilView* mDepthStencilView = nullptr;
	ID3D11Buffer* mConstantBuffer = nullptr;

	Eye mEye = {
		{0.0f, 1.6f, -10.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f},
		{1.0f, 0.0f, 0.0f}
	};
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;

	float mSpeed = 5.0f;

	CubeBox cubeBox;
	ID3D11Buffer* mCubeBoxWorldConstantBuffer = nullptr;
	GroundSurface surface;
	ID3D11Buffer* mSurfaceWorldConstantBuffer = nullptr;
		
public:
	App() : mMainWindow(new MainWindow()) {};

	int Initialize();
	int Run();
};
