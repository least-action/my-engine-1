#include "App.h"
#include "MainWindow.h"
#include "CubeBox.h"
#include "MathUtils.h"

#include <D3DCompiler.h>
#include <directxmath.h>

#include <stdio.h>

HRESULT App::InitWindow()
{
    if (!mMainWindow->Create(L"my-engine-1", WS_OVERLAPPEDWINDOW)) {
        printf("failed to create main window\n");
        return E_FAIL;
    }

	ShowWindow(mMainWindow->Window(), SW_SHOWDEFAULT);

	return S_OK;
}

HRESULT App::InitD3D()
{
    HRESULT hr;

    RECT rt;
    GetClientRect(mMainWindow->Window(), &rt);
    UINT width = rt.right - rt.left;
    UINT height = rt.bottom - rt.top;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = mMainWindow->Window();
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };
    
    D3D_FEATURE_LEVEL maxSupportedFeatureLevel;

    UINT creationFlags = 0;
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        &sd,
        &mSwapChain,
        &mDevice,
        &maxSupportedFeatureLevel,
        &mContext);

    if (FAILED(hr)) {
        printf("D3D11CreateDeviceAndSwapChain error : %08X\n", hr);
        return hr;
    }

    // Create a render target view
    ID3D11Texture2D* pBackBuffer;
    hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr)) {
        printf("GetBuffer error : %08X\n", hr);
        return hr;
    }

    hr = mDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr)) {
        printf("CreateRenderTargetView error : %08X\n", hr);
        return hr;
    }

    
    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = mDevice->CreateTexture2D(&descDepth, nullptr, &mDepthStencil);
    if (FAILED(hr)) {
        printf("CreateTexture2D error : %08X\n", hr);
        return hr;
    }
        
    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = mDevice->CreateDepthStencilView(mDepthStencil, &descDSV, &mDepthStencilView);
    if (FAILED(hr)) {
        printf("CreateDepthStencilView error : %08X\n", hr);
        return hr;
    }

    mContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

    
    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    mContext->RSSetViewports(1, &vp);

    surface.Initialize(mDevice, mContext);
    cubeBox.Initialize(mDevice, mContext);
    
    // Create the constant buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = mDevice->CreateBuffer(&bd, NULL, &mConstantBuffer);
    if (FAILED(hr)) {
        printf("CreateBuffer(constant buffer) error : %08X\n", hr);
        return hr;
    }

    // Initialize the world matrices
    mWorld = DirectX::XMMatrixIdentity();

    // Initialize the view matrix
    mView = MathUtils::MatrixLookAtLH(mEye.Pos, mEye.Look, mEye.Up);
    

    // Initialize the projection matrix
    mProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);

	return S_OK;
}

void App::CleanupDevice()
{
    if (mContext) mContext->ClearState();

    if (mConstantBuffer) mConstantBuffer->Release();
    // if (mVertexBuffer) mVertexBuffer->Release();
    // if (mIndexBuffer) mIndexBuffer->Release();
    // if (mInputLayout) mInputLayout->Release();
    // if (mVertexShader) mVertexShader->Release();
    // if (mPixelShaderSolid) mPixelShaderSolid->Release();
    // if (mPixelShader) mPixelShader->Release();
    if (mDepthStencil) mDepthStencil->Release();
    if (mDepthStencilView) mDepthStencilView->Release();
    if (mRenderTargetView) mRenderTargetView->Release();
    if (mSwapChain) mSwapChain->Release();
    if (mContext) mContext->Release();
    if (mDevice) mDevice->Release();
	return;
}

int App::Initialize()
{
    if (FAILED(InitWindow()))
        return 0;

    if (FAILED(InitD3D())) {
        CleanupDevice();
        return 0;
    }

    return 0;
}

void App::UpdateModels()
{
    // Update our time
    static float t = 0.0f;
    static DWORD dwTimeStart = GetTickCount64();
    DWORD dwTimeCur = GetTickCount64();
    t = (dwTimeCur - dwTimeStart) / 1000.0f;
}

void App::Render()
{
    UpdateModels();

    // Rotate cube around the origin
    mWorld = DirectX::XMMatrixRotationY(0);

    // Setup our lighting parameters
    DirectX::XMFLOAT4 vLightDirs[2] =
    {
        DirectX::XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f),
    };
    DirectX::XMFLOAT4 vLightColors[2] =
    {
        DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f)
    };

    // Rotate the second light around the origin
    DirectX::XMMATRIX mRotate = DirectX::XMMatrixRotationY(-2.0f * 10);
    DirectX::XMVECTOR vLightDir = DirectX::XMLoadFloat4(&vLightDirs[1]);
    vLightDir = DirectX::XMVector3Transform(vLightDir, mRotate);
    DirectX::XMStoreFloat4(&vLightDirs[1], vLightDir);

    //
    // Clear the back buffer
    //
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
    mContext->ClearRenderTargetView(mRenderTargetView, ClearColor);

    //
    // Clear the depth buffer to 1.0 (max depth)
    //
    mContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    //
    // Update matrix variables and lighting variables
    //
    ConstantBuffer cb1;
    cb1.mWorld = DirectX::XMMatrixTranspose(mWorld);
    cb1.mView = DirectX::XMMatrixTranspose(mView);
    cb1.mProjection = DirectX::XMMatrixTranspose(mProjection);
    cb1.vLightDir[0] = vLightDirs[0];
    cb1.vLightDir[1] = vLightDirs[1];
    cb1.vLightColor[0] = vLightColors[0];
    cb1.vLightColor[1] = vLightColors[1];
    cb1.vOutputColor = DirectX::XMFLOAT4(0, 0, 0, 0);
    mContext->UpdateSubresource(mConstantBuffer, 0, NULL, &cb1, 0, 0);

    //
    // Render the ground surface
    //
    surface.Render(mContext, mConstantBuffer);

    //
    // Render the cube
    //
    cubeBox.Render(mContext, mConstantBuffer);

    //
    // Present our back buffer to our front buffer
    //
    mSwapChain->Present(0, 0);
}

int App::Run()
{
	MSG msg = { };
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            Render();
        }
	}

	return 0;
}