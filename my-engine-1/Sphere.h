#pragma once
#include "D3DUtils.h"
#include "MathUtils.h"

#include <d3d11.h>
#include <directxmath.h>
#include <D3DCompiler.h>

#include <stdexcept>
#include <stdio.h>

class Sphere {
    float radius;
    struct SimpleVertex
    {
        MathUtils::Point Pos;
        MathUtils::Vector Normal;
        MathUtils::TextCoord TextCoord;
    };

    struct WorldContantBuffer
    {
        MathUtils::Matrix mWorld;
    };

    struct Model
    {
        MathUtils::Point Pos;
        MathUtils::Vector RotateAxis = { 0.0f, 1.0f, 0.0f };
        float RotateRadian = 0;
    };

    ID3D11Buffer* mVertexBuffer = nullptr;
    ID3D11Buffer* mIndexBuffer = nullptr;
    ID3D11Buffer* worldContantBuffer = nullptr;
    ID3D11VertexShader* mVertexShader = nullptr;
    ID3D11PixelShader* mPixelShader = nullptr;
    ID3D11InputLayout* mInputLayout = nullptr;
    
    ID3D11Texture2D* mTexture = nullptr;
    ID3D11ShaderResourceView* mTextureResourceView = nullptr;

    int thetaDivision, phiDivision;
    int indiciesNum = 0;

public:
    Sphere(float radius, int thetaDivision, int phiDivision)
    {
        if (!(radius > 0))
            throw std::runtime_error("radius must be greater than 0");
        if (thetaDivision < 2 || phiDivision < 2)
            throw std::runtime_error("division should be greater than 2");
        this->radius = radius;
        this->thetaDivision = thetaDivision;
        this->phiDivision = phiDivision;
        model.Pos.y = radius;
    }

    Model model = Model();

    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
    {
        D3DUtils::CreateTexture(device, "earth.png", &mTexture, &mTextureResourceView, DXGI_FORMAT_R8G8B8A8_UNORM);

        // build verticies and indices
        std::vector<SimpleVertex> verticies;
        int verticiesNum = 2 * thetaDivision + (thetaDivision + 1) * (phiDivision - 1);
        verticies.reserve(verticiesNum);
        std::vector<WORD> indicies;
        int indiciesNum = (thetaDivision * 2 * 3) + (thetaDivision * (phiDivision - 2) * 6);
        indicies.reserve(indiciesNum);
        this->indiciesNum = indiciesNum;

        float deltaTheta = 2.0f * MathUtils::PI / thetaDivision;
        float deltaPhi = MathUtils::PI / phiDivision;
        
        // verticies
        SimpleVertex top, bottom;
        top.Pos = {0.0f, radius, 0.0f};
        top.Normal = {0.0f, 1.0f, 0.0f};
        for (int j = 0; j < thetaDivision; ++j) {
            top.TextCoord = { 1.0f / (thetaDivision - 1) * j, 0.0f };
            verticies.push_back(top);
        }
        for (int i = 1; i < phiDivision; ++i) {
            float y = radius * cos(deltaPhi * i);
            float xzRadius = radius * sin(deltaPhi * i);
            SimpleVertex a;
            for (int j = 0; j < thetaDivision + 1; ++j) {
                a.Pos = { float(xzRadius * cos(j * deltaTheta)), y, float(xzRadius * sin(j * deltaTheta))};
                a.Normal = (a.Pos - MathUtils::Point{0.0f, 0.0f, 0.0f}).Normalized();
                a.TextCoord = {1.0f / thetaDivision * j, 1.0f / phiDivision * i };
                verticies.push_back(a);
            }
        }
        bottom.Pos = { 0.0f, -radius, 0.0f };
        bottom.Normal = { 0.0f, -1.0f, 0.0f };
        for (int j = 0; j < thetaDivision; ++j) {
            bottom.TextCoord = { 1.0f / (thetaDivision - 1) * j, 1.0f };
            verticies.push_back(bottom);
        }

        // indices
        for (int j = 0; j < thetaDivision; ++j) {
            indicies.push_back(j);
            indicies.push_back(j + thetaDivision + 1);
            indicies.push_back(j + thetaDivision);
        }

        for (int i = 0; i < phiDivision - 2; ++i) {
            int startOfUp = i * (thetaDivision + 1) + thetaDivision;
            int startOfDown = startOfUp + thetaDivision + 1;

            for (int j = 0; j < thetaDivision; ++j) {
                indicies.push_back(startOfUp + j);
                indicies.push_back(startOfUp + j + 1);
                indicies.push_back(startOfDown + j);

                indicies.push_back(startOfUp + j + 1);
                indicies.push_back(startOfDown + j + 1);
                indicies.push_back(startOfDown + j);
            }

        }

        int startOfUp = verticiesNum - 1 - (thetaDivision - 1) - (thetaDivision + 1);
        for (int j = 0; j < thetaDivision; ++j) {
            indicies.push_back(startOfUp + j);
            indicies.push_back(startOfUp + 1 + j);
            indicies.push_back(startOfUp + j + thetaDivision + 1);
        }


        //// Define the input layout
        //std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
        //{
        //    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        //    { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        //    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        //};
        //D3DUtils::CreateVertexShaderWithInputLayout(device, L"shaderEarthVertex.hlsl", &mVertexShader, layout, &mInputLayout);
        //D3DUtils::CreatePixelShader(device, L"shaderEarthPixel.hlsl", &mPixelShader);

        D3DUtils::CreateVertexBufferWithIndexBuffer(
            device,
            &mVertexBuffer, sizeof(SimpleVertex) * verticiesNum, verticies.data(),
            &mIndexBuffer, sizeof(WORD) * indiciesNum, indicies.data()
        );

        // Create the constant buffer
        HRESULT hr;
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WorldContantBuffer);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = device->CreateBuffer(&bd, NULL, &worldContantBuffer);
        if (FAILED(hr)) {
            printf("CreateBuffer(mSurfaceWorldConstantBuffer) error : %08X\n", hr);
            throw std::runtime_error("");
        }
    }

    void Render(ID3D11DeviceContext* context, ID3D11Buffer* sharedContantBuffer)
    {
        WorldContantBuffer wb;
        auto transition = MathUtils::Matrix(
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                model.Pos.x, model.Pos.y, model.Pos.z, 1.0f,
            }
        );
        auto rotation = MathUtils::BuildRotation(model.RotateAxis, model.RotateRadian);
        wb.mWorld = (transition * rotation).Transposed();
        context->UpdateSubresource(worldContantBuffer, 0, NULL, &wb, 0, 0);

        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

        // Set index buffer
        context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

        // Set primitive topology
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->VSSetConstantBuffers(0, 1, &sharedContantBuffer);
        context->VSSetConstantBuffers(1, 1, &worldContantBuffer);
        context->PSSetConstantBuffers(0, 1, &sharedContantBuffer);
        context->PSSetShaderResources(0, 1, &mTextureResourceView);
        context->DrawIndexed(indiciesNum, 0, 0);
    }
};
