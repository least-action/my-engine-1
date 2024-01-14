#pragma once
#include "MainWindow.h"
#include "CubeBox.h"
#include "Sphere.h"
#include "GroundSurface.h"
#include "MathUtils.h"
#include <windows.h>
#include <directxmath.h>


#include <memory>

class App {
	struct ConstantBuffer
	{
		MathUtils::Matrix mView;
		MathUtils::Matrix mProjection;
		DirectX::XMFLOAT4 vLightDir[2];
		DirectX::XMFLOAT4 vLightColor[2];
		DirectX::XMFLOAT4 vOutputColor;
		MathUtils::Point mBlackholePos;
	};

	struct WorldContantBuffer
	{
		MathUtils::Matrix mWorld;
	};

	struct Camera
	{
		MathUtils::Point Pos;
		MathUtils::Vector Look;
		MathUtils::Vector Up;
	};
	Camera mCamera = {
		{0.0f, 1.6f, -20.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f}
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

	MathUtils::Matrix mView;
	MathUtils::Matrix mProjection;
	MathUtils::Point mBlackholePos = {0.0f, 0.0f, -10.0f};

	float mSpeed = 10.0f;

	CubeBox cubeBox;
	GroundSurface surface;
	Sphere sphere;
		
public:
	App() : mMainWindow(new MainWindow()) {};

	int Initialize();
	int Run();
};
