#include "App.h"
#include "MainWindow.h"
#include "CubeBox.h"
#include "MathUtils.h"

#include <D3DCompiler.h>
#include <directxmath.h>

#include <iostream>
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

    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(rasterDesc));
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthClipEnable = true;
    mDevice->CreateRasterizerState(&rasterDesc, &mDefaultRasterizer);

    ZeroMemory(&rasterDesc, sizeof(rasterDesc));
    rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthClipEnable = true;
    mDevice->CreateRasterizerState(&rasterDesc, &mWireRasterizer);

    ZeroMemory(&rasterDesc, sizeof(rasterDesc));
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthClipEnable = false;
    mDevice->CreateRasterizerState(&rasterDesc, &mNoneRasterizer);
    
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
    sphere.Initialize(mDevice, mContext);
    cubeMap.Initialize(mDevice, mContext);
    
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

    // Initialize the view matrix
    View = MathUtils::MatrixLookAtLH(mCamera.Pos, mCamera.Look, mCamera.Up, mCamera.Up.Cross(mCamera.Look));
    
    // Initialize the projection matrix
    Projection = MathUtils::MatrixPerspectiveForLH(DirectX::XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.1f);

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
    static float dt = 0.0f;
    static DWORD dwTimeStart = GetTickCount64();
    static DWORD prevTime = GetTickCount64();
    DWORD dwTimeCur = GetTickCount64();
    
    t = (dwTimeCur - dwTimeStart) / 1000.0f;
    dt = (dwTimeCur - prevTime) / 1000.0f;
    prevTime = dwTimeCur;
    
    static bool isRightClickJustClick = true;
    static POINT startPoint;
    static POINT currentPoint;
    
    static MathUtils::Vector changedLook = mCamera.Look;
    static MathUtils::Vector changedUp = mCamera.Up;
    static MathUtils::Vector changedRight = mCamera.Up.Cross(mCamera.Look);

    MathUtils::Vector cameraRight = mCamera.Up.Cross(mCamera.Look);
    
    if (mMainWindow->IsRightClickDown()) {
        GetCursorPos(&currentPoint);
        if (isRightClickJustClick) {
            startPoint = currentPoint;
            isRightClickJustClick = false;
        }
        
        float deltaYaw = (currentPoint.x - startPoint.x) / 360.0f;
        float deltaPitch = -(currentPoint.y - startPoint.y) / 360.0f;

        MathUtils::Matrix rotationYaw = MathUtils::BuildRotation({ 0.0f, 1.0f, 0.0f }, deltaYaw);
        changedRight = cameraRight * rotationYaw;

        MathUtils::Matrix rotationPitch = MathUtils::BuildRotation(changedRight, -deltaPitch);
        MathUtils::Matrix rotationMatrix = rotationYaw * rotationPitch;

        changedLook = mCamera.Look * rotationMatrix;
        changedUp = mCamera.Up * rotationMatrix;
    }
    else {
        if (!isRightClickJustClick) {
            isRightClickJustClick = true;

            mCamera.Look = changedLook;
            mCamera.Up = changedUp;

            mCamera.Look = mCamera.Look * (1.0f / mCamera.Look.length());

            mCamera.Up = mCamera.Up - (mCamera.Look * mCamera.Look.Dot(mCamera.Up));
            mCamera.Up = mCamera.Up * (1.0f / mCamera.Up.length());
        }
        changedLook = mCamera.Look;
        changedUp = mCamera.Up;
    }

    MathUtils::Vector movingDir = { 0.0f, 0.0f, 0.0f };
    if (mMainWindow->IsRightDown() && !mMainWindow->IsLeftDown()) {
        movingDir = movingDir + changedRight;
    }
    else if (!mMainWindow->IsRightDown() && mMainWindow->IsLeftDown()) {
        movingDir = movingDir - changedRight;
    }
    if (mMainWindow->IsUpDown() && !mMainWindow->IsDownDown()) {
        movingDir = movingDir + changedLook;
    }
    else if (!mMainWindow->IsUpDown() && mMainWindow->IsDownDown()) {
        movingDir = movingDir - changedLook;
    }
    mCamera.Pos = mCamera.Pos + (movingDir * (dt * mSpeed));

    View = MathUtils::MatrixLookAtLH(mCamera.Pos, changedLook, changedUp, changedUp.Cross(changedLook));
    mViewOnlyRotation = MathUtils::MatrixLookAtLH({ 0.0f, 0.0f, 0.0f }, changedLook, changedUp, changedUp.Cross(changedLook));

    // Update cube
    cubeBox.model.Pos = {5.0f * cos(t * 0.5f), 0.0f, 5.0f * sin(t * 0.5f)};
    sphere.model.RotateRadian = -t;
}

void App::Render()
{
    UpdateModels();

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
    cb1.View = View.Transposed();
    cb1.Projection = Projection.Transposed();
    cb1.Light1 = mPointLight1;
    cb1.Light2 = mPointLight2;
    mContext->UpdateSubresource(mConstantBuffer, 0, NULL, &cb1, 0, 0);

    //
    // Render
    //
    surface.Render(mContext, mConstantBuffer);
    cubeBox.Render(mContext, mConstantBuffer);
    sphere.Render(mContext, mConstantBuffer, mWireRasterizer, mDefaultRasterizer);
    //cubeMap.Render(mContext, mViewOnlyRotation, mProjection, mConstantBuffer);
    cubeMap.Render(mContext, mViewOnlyRotation, mConstantBuffer, mNoneRasterizer, mDefaultRasterizer);

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