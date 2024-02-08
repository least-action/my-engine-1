#pragma once
#include "MainWindow.h"
#include "CubeBox.h"
#include "LightPosition.h"
#include "Sphere.h"
#include "GroundSurface.h"
#include "CubeMap.h"
#include "MathUtils.h"
#include "Light.h"
#include "PipelineStateObject.h"
#include <windows.h>
#include <directxmath.h>


#include <memory>

class App {
	struct ConstantBuffer
	{
		MathUtils::Matrix mView;
		MathUtils::Matrix mProjection;
		MathUtils::Matrix InvProjection;
		Light Light1;
		Light Light2;
	};

	struct WorldContantBuffer
	{
		MathUtils::Matrix World;
	};

	struct Camera
	{
		MathUtils::Point Pos;
		MathUtils::Vector Look;
		MathUtils::Vector Up;
	};

	Camera mCamera = {
		{0.0f, MathUtils::SCALE * 0.75f, -MathUtils::SCALE * 1.3f},
		{0.0f, -0.374015f, 0.927423f},
		{0.0f, 0.927423f, 0.374015f}
	};

	HRESULT InitWindow();
	HRESULT InitD3D();
	void CleanupDevice();

	void SetPSO(PipelineStateObject pso);
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


	std::vector<D3D11_INPUT_ELEMENT_DESC> mSolidILDesc =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	ID3D11VertexShader* mSolidVS = nullptr;
	ID3D11PixelShader* mSolidPS = nullptr;
	ID3D11InputLayout* mSolidIL = nullptr;
	ID3D11RasterizerState* mSolidRS = nullptr;
	PipelineStateObject mSolidPSO;

	ID3D11PixelShader* mLightPS = nullptr;
	PipelineStateObject mLightPSO;


	ID3D11PixelShader* mWirePS = nullptr;
	ID3D11RasterizerState* mWireRS = nullptr;
	PipelineStateObject mWirePSO;


	ID3D11VertexShader* mGroundVS = nullptr;
	ID3D11PixelShader* mGroundPS = nullptr;
	PipelineStateObject mGroundPSO;


	ID3D11VertexShader* mCubeMapVS = nullptr;
	ID3D11PixelShader* mCubeMapPS = nullptr;
	ID3D11InputLayout* mCubeMapIL = nullptr;
	ID3D11RasterizerState* mCubeMapRS = nullptr;
	PipelineStateObject mCubeMapPSO;
	

	ID3D11PixelShader* mDepthOnlyPS = nullptr;
	ID3D11SamplerState* mShadowPointSS = nullptr;
	ID3D11Texture2D* mDepthOnlyTexture = nullptr;
	ID3D11DepthStencilView* mDepthOnlyDSV = nullptr;
	ID3D11ShaderResourceView* mDepthOnlySRV = nullptr;

	ID3D11Texture2D* mDepthOnlyTexture2 = nullptr;
	ID3D11DepthStencilView* mDepthOnlyDSV2 = nullptr;
	ID3D11ShaderResourceView* mDepthOnlySRV2 = nullptr;


	MathUtils::Matrix mView;
	MathUtils::Matrix mViewOnlyRotation;
	MathUtils::Matrix mProjection;
	MathUtils::Matrix InvProjection;

	float mSpeed = MathUtils::SCALE * 0.5f;

	CubeBox cubeBox;
	LightPosition l1;
	LightPosition l2;
	GroundSurface surface;
	Sphere sphere{ MathUtils::SCALE * 0.25f, 10, 10 };
	CubeMap cubeMap{ MathUtils::SCALE * 5.0f, 30, 30 };
	
	Light mPointLight1{ { -MathUtils::SCALE * 1.0f, MathUtils::SCALE * 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, MathUtils::SCALE * MathUtils::SCALE * 1.2f, 0 };
	Light mPointLight2{ { MathUtils::SCALE * 0.4f, MathUtils::SCALE * 0.15f, 0.0f }, { 1.0f, 1.0f, 1.0f }, MathUtils::SCALE * MathUtils::SCALE * 0.5f, 0 };

public:
	App() : mMainWindow(new MainWindow()) {};

	int Initialize();
	int Run();
};
