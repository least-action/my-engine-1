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
    
    // Create depth stencil texture
    {
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
    }
    
    // Setup the viewport
    {
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
        l1.Initialize(mDevice, mContext);
        l2.Initialize(mDevice, mContext);
    }
    
    // Create the constant buffer
    {
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
    }
    

    D3D11_RASTERIZER_DESC rasterDesc;

    // Solid PSO
    {
        D3DUtils::CreateVertexShaderWithInputLayout(mDevice, L"shaderDefaultVertex.hlsl", &mSolidVS, mSolidILDesc, &mSolidIL);
        D3DUtils::CreatePixelShader(mDevice, L"shaderDefaultPixel.hlsl", &mSolidPS);
        ZeroMemory(&rasterDesc, sizeof(rasterDesc));
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.DepthClipEnable = false;
        mDevice->CreateRasterizerState(&rasterDesc, &mSolidRS);

        mSolidPSO.IL = mSolidIL;
        mSolidPSO.VS = mSolidVS;
        mSolidPSO.PS = mSolidPS;
        mSolidPSO.RS = mSolidRS;
    }

    // Light PSO
    {
        D3DUtils::CreatePixelShader(mDevice, L"shaderLightPositionPixel.hlsl", &mLightPS);
        
        mLightPSO.IL = mSolidIL;
        mLightPSO.VS = mSolidVS;
        mLightPSO.PS = mLightPS;
        mLightPSO.RS = mSolidRS;
    }
    
    // Wire PSO
    {
        D3DUtils::CreatePixelShader(mDevice, L"shaderDefaultWirePixel.hlsl", &mWirePS);
        ZeroMemory(&rasterDesc, sizeof(rasterDesc));
        rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.DepthClipEnable = true;
        mDevice->CreateRasterizerState(&rasterDesc, &mWireRS);
        mWirePSO.IL = mSolidIL;
        mWirePSO.VS = mSolidVS;
        mWirePSO.PS = mWirePS;
        mWirePSO.RS = mWireRS;
    }

    // Sphere PSO
    {
        D3DUtils::CreateVertexShaderWithInputLayout(mDevice, L"shaderSphereVertex.hlsl", &mSphereVS, mSolidILDesc, &mSphereIL);
        D3DUtils::CreateGeometryShader(mDevice, L"shaderSphereGeometry.hlsl", &mSphereGS);
        mSpherePSO.IL = mSphereIL;
        mSpherePSO.VS = mSphereVS;
        mSpherePSO.GS = mSphereGS;
        mSpherePSO.PS = mSolidPS;
        mSpherePSO.RS = mSolidRS;
    }

    // Ground PSO
    {
        D3DUtils::CreatePixelShader(mDevice, L"shaderGroundPixel.hlsl", &mGroundPS);
        mGroundPSO.IL = mSolidIL;
        mGroundPSO.VS = mSolidVS;
        mGroundPSO.PS = mGroundPS;
        mGroundPSO.RS = mSolidRS;
    }

    // CubeMap PSO
    {
        std::vector<D3D11_INPUT_ELEMENT_DESC> cubeMaplayout =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        D3DUtils::CreateVertexShaderWithInputLayout(mDevice, L"shaderCubeMapVertex.hlsl", &mCubeMapVS, cubeMaplayout, &mCubeMapIL);
        D3DUtils::CreatePixelShader(mDevice, L"shaderCubeMapPixel.hlsl", &mCubeMapPS);
        ZeroMemory(&rasterDesc, sizeof(rasterDesc));
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.DepthClipEnable = false;
        mDevice->CreateRasterizerState(&rasterDesc, &mCubeMapRS);

        mCubeMapPSO.IL = mCubeMapIL;
        mCubeMapPSO.VS = mCubeMapVS;
        mCubeMapPSO.PS = mCubeMapPS;
        mCubeMapPSO.RS = mCubeMapRS;
    }

    // Shadow
    {
        D3DUtils::CreatePixelShader(mDevice, L"shaderDepthPixel.hlsl", &mDepthOnlyPS);
        
        // Create depth texture
        D3D11_TEXTURE2D_DESC descDepth;
        ZeroMemory(&descDepth, sizeof(descDepth));
        descDepth.Width = width;
        descDepth.Height = height;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
        descDepth.SampleDesc.Count = 1;
        descDepth.SampleDesc.Quality = 0;
        descDepth.Usage = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        //descDepth.MiscFlags = 0;
        
        hr = mDevice->CreateTexture2D(&descDepth, nullptr, &mDepthOnlyTexture);
        if (FAILED(hr)) {
            printf("CreateTexture2D890 error : %08X\n", hr);
            return hr;
        }
        hr = mDevice->CreateTexture2D(&descDepth, nullptr, &mDepthOnlyTexture2);
        if (FAILED(hr)) {
            printf("CreateTexture2D890 error : %08X\n", hr);
            return hr;
        }

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        ZeroMemory(&descDSV, sizeof(descDSV));
        descDSV.Format = DXGI_FORMAT_D32_FLOAT;
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        //descDSV.Texture2D.MipSlice = 0;
        hr = mDevice->CreateDepthStencilView(mDepthOnlyTexture, &descDSV, &mDepthOnlyDSV);
        if (FAILED(hr)) {
            printf("CreateDepthStencilView456 error : %08X\n", hr);
            return hr;
        }
        hr = mDevice->CreateDepthStencilView(mDepthOnlyTexture2, &descDSV, &mDepthOnlyDSV2);
        if (FAILED(hr)) {
            printf("CreateDepthStencilView456 error : %08X\n", hr);
            return hr;
        }

        // Create shader resource view
        D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
        memset(&descSRV, 0, sizeof(descSRV));
        descSRV.Format = DXGI_FORMAT_R32_FLOAT;
        descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        descSRV.Texture2D.MipLevels = 1;

        hr = mDevice->CreateShaderResourceView(mDepthOnlyTexture, &descSRV, &mDepthOnlySRV);
        if (FAILED(hr))
        {
            //mDepthOnlyTexture->Release();
            printf("CreateShaderResourceView123 error : %08X\n", hr);
            throw std::runtime_error("");
        }
        hr = mDevice->CreateShaderResourceView(mDepthOnlyTexture2, &descSRV, &mDepthOnlySRV2);
        if (FAILED(hr))
        {
            //mDepthOnlyTexture->Release();
            printf("CreateShaderResourceView123 error : %08X\n", hr);
            throw std::runtime_error("");
        }


        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory(&sampDesc, sizeof(sampDesc));
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        
        mDevice->CreateSamplerState(&sampDesc, &mShadowPointSS);
    }
    

    // Initialize the projection matrix
    mProjection = MathUtils::MatrixPerspectiveForLH(DirectX::XM_PIDIV4, width / (FLOAT)height, MathUtils::SCALE * 0.1f, MathUtils::SCALE * 5.0f);
    InvProjection = MathUtils::InvMatrixPerspectiveForLH(DirectX::XM_PIDIV4, width / (FLOAT)height, MathUtils::SCALE * 0.1f, MathUtils::SCALE * 5.0f);

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

    mView = MathUtils::MatrixLookAtLH(mCamera.Pos, changedLook, changedUp, changedUp.Cross(changedLook));
    mViewOnlyRotation = MathUtils::MatrixLookAtLH({ 0.0f, 0.0f, 0.0f }, changedLook, changedUp, changedUp.Cross(changedLook));
    


    // Update cube
    cubeBox.model.Pos = { MathUtils::SCALE * 0.25f * cos(t * 0.5f), 0.0f, MathUtils::SCALE * 0.25f * sin(t * 0.5f)};
    mPointLight2.Pos = { MathUtils::SCALE * 0.6f * cos(t * 0.5f), MathUtils::SCALE * 0.5f, MathUtils::SCALE * 0.6f * -sin(t * 0.5f) };
    sphere.model.RotateRadian = -t;
    mPointLight1.View = MathUtils::MatrixLookAtLH(mPointLight1.Pos, { 0.70710678f, -0.70710678f, 0.0f }, { 0.70710678f, 0.70710678f, 0.0f }, { 0.0f, 0.0f, -1.0f });
    MathUtils::Vector light2Look = (MathUtils::Point{} -  mPointLight2.Pos).Normalized();
    float xzLength = pow(light2Look.x * light2Look.x + light2Look.z * light2Look.z, 0.5);
    MathUtils::Vector light2Up = {light2Look.x * -light2Look.y / xzLength, xzLength, light2Look.z * -light2Look.y / xzLength};
    mPointLight2.View = MathUtils::MatrixLookAtLH(mPointLight2.Pos, light2Look, light2Up, light2Up.Cross(light2Look));
    l1.model.Pos = mPointLight1.Pos;
    l2.model.Pos = mPointLight2.Pos;
}

void App::SetPSO(PipelineStateObject pso)
{
    mContext->IASetInputLayout(pso.IL);
    mContext->VSSetShader(pso.VS, nullptr, 0);
    mContext->GSSetShader(pso.GS, nullptr, 0);
    mContext->PSSetShader(pso.PS, nullptr, 0);
    mContext->RSSetState(pso.RS);
}

void App::Render()
{
    UpdateModels();

    //
    // Clear the back buffer
    //
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
    mContext->ClearRenderTargetView(mRenderTargetView, ClearColor);

    //
    // Clear the depth buffer to 1.0 (max depth)
    //
    mContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    mContext->ClearDepthStencilView(mDepthOnlyDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
    mContext->ClearDepthStencilView(mDepthOnlyDSV2, D3D11_CLEAR_DEPTH, 1.0f, 0);

    //
    // Update matrix variables and lighting variables
    //
    ConstantBuffer cb1;
    cb1.mView = mPointLight1.View.Transposed();
    cb1.mProjection = mProjection.Transposed();
    cb1.InvProjection = InvProjection.Transposed();
    cb1.Light1 = mPointLight1;
    cb1.Light2 = mPointLight2;
    mContext->UpdateSubresource(mConstantBuffer, 0, NULL, &cb1, 0, 0);

    //
    // Create Depth Only Texture
    //
    mContext->OMSetRenderTargets(0, nullptr, mDepthOnlyDSV);
    SetPSO(mSolidPSO);
    mContext->PSSetShader(mDepthOnlyPS, nullptr, 0);
    mContext->PSSetSamplers(0, 1, &mShadowPointSS);
    cubeBox.Render(mContext, mConstantBuffer);
    sphere.Render(mContext, mConstantBuffer);
    SetPSO(mGroundPSO);
    mContext->PSSetShader(mDepthOnlyPS, nullptr, 0);
    mContext->PSSetSamplers(0, 1, &mShadowPointSS);
    surface.Render(mContext, mConstantBuffer);

    
    cb1.mView = mPointLight2.View.Transposed();
    cb1.mProjection = mProjection.Transposed();
    cb1.InvProjection = InvProjection.Transposed();
    cb1.Light1 = mPointLight1;
    cb1.Light2 = mPointLight2;
    mContext->UpdateSubresource(mConstantBuffer, 0, NULL, &cb1, 0, 0);

    mContext->OMSetRenderTargets(0, nullptr, mDepthOnlyDSV2);
    SetPSO(mSolidPSO);
    mContext->PSSetShader(mDepthOnlyPS, nullptr, 0);
    mContext->PSSetSamplers(0, 1, &mShadowPointSS);
    cubeBox.Render(mContext, mConstantBuffer);
    //SetPSO(mSpherePSO);
    sphere.Render(mContext, mConstantBuffer);
    SetPSO(mGroundPSO);
    mContext->PSSetShader(mDepthOnlyPS, nullptr, 0);
    mContext->PSSetSamplers(0, 1, &mShadowPointSS);
    surface.Render(mContext, mConstantBuffer);
    
    mContext->PSSetShaderResources(10, 1, &mDepthOnlySRV);
    mContext->PSSetShaderResources(11, 1, &mDepthOnlySRV2);
    
    //
    // Update matrix variables and lighting variables
    //
    cb1.mView = mView.Transposed();
    cb1.mProjection = mProjection.Transposed();
    cb1.InvProjection = InvProjection.Transposed();
    cb1.Light1 = mPointLight1;
    cb1.Light2 = mPointLight2;
    mContext->UpdateSubresource(mConstantBuffer, 0, NULL, &cb1, 0, 0);

    //
    // Render
    //
    mContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

    SetPSO(mSolidPSO);
    //SetPSO(mWirePSO);
    mContext->PSSetShaderResources(10, 1, &mDepthOnlySRV);
    mContext->PSSetShaderResources(11, 1, &mDepthOnlySRV2);
    cubeBox.Render(mContext, mConstantBuffer);
    //SetPSO(mWirePSO);
    SetPSO(mSpherePSO);
    sphere.Render(mContext, mConstantBuffer);
    SetPSO(mLightPSO);
    l1.Render(mContext, mConstantBuffer);
    l2.Render(mContext, mConstantBuffer);
    
    SetPSO(mGroundPSO);
    mContext->PSSetShaderResources(10, 1, &mDepthOnlySRV);
    mContext->PSSetShaderResources(11, 1, &mDepthOnlySRV2);
    surface.Render(mContext, mConstantBuffer);
    
    SetPSO(mCubeMapPSO);
    cubeMap.Render(mContext, mViewOnlyRotation, mConstantBuffer);

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