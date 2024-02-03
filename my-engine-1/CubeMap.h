#pragma once
#include <d3d11.h>
#include <stdexcept>
#include "MathUtils.h"
#include "D3DUtils.h"


class CubeMap {
    float radius;
    struct SimpleVertex
    {
        MathUtils::Point Pos;
    };

    struct ViewConstantBuffer
    {
        MathUtils::Matrix Rotation;
    };

    struct Model
    {
        MathUtils::Point Pos;
        MathUtils::Vector RotateAxis = { 0.0f, 1.0f, 0.0f };
        float RotateRadian = 0;
    };

    ID3D11Buffer* mVertexBuffer = nullptr;
    ID3D11Buffer* mIndexBuffer = nullptr;
    ID3D11Buffer* mViewContantBuffer = nullptr;
    ID3D11VertexShader* mVertexShader = nullptr;
    ID3D11PixelShader* mPixelShader = nullptr;
    ID3D11InputLayout* mInputLayout = nullptr;
    
    ID3D11Texture2D* mTextureXP = nullptr;
    ID3D11Texture2D* mTextureXN = nullptr;
    ID3D11Texture2D* mTextureZP = nullptr;
    ID3D11Texture2D* mTextureZN = nullptr;
    ID3D11Texture2D* mTextureYP = nullptr;
    ID3D11Texture2D* mTextureYN = nullptr;
    ID3D11ShaderResourceView* mTextureResourceViewXP = nullptr;
    ID3D11ShaderResourceView* mTextureResourceViewXN = nullptr;
    ID3D11ShaderResourceView* mTextureResourceViewZP = nullptr;
    ID3D11ShaderResourceView* mTextureResourceViewZN = nullptr;
    ID3D11ShaderResourceView* mTextureResourceViewYP = nullptr;
    ID3D11ShaderResourceView* mTextureResourceViewYN = nullptr;


    int thetaDivision, phiDivision;
    int indiciesNum = 0;

public:
    CubeMap(float radius, int thetaDivision, int phiDivision)
    {
        if (!(radius > 0))
            throw std::runtime_error("radius must be greater than 0");
        if (thetaDivision < 2 || phiDivision < 2)
            throw std::runtime_error("division should be greater than 2");
        this->radius = radius;
        this->thetaDivision = thetaDivision;
        this->phiDivision = phiDivision;
    }

    Model model = Model();

    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
    {
        D3DUtils::CreateTexture(device, "right.png", &mTextureXP, &mTextureResourceViewXP, DXGI_FORMAT_R8G8B8A8_UNORM);
        D3DUtils::CreateTexture(device, "left.png", &mTextureXN, &mTextureResourceViewXN, DXGI_FORMAT_R8G8B8A8_UNORM);
        D3DUtils::CreateTexture(device, "front.png", &mTextureZP, &mTextureResourceViewZP, DXGI_FORMAT_R8G8B8A8_UNORM);
        D3DUtils::CreateTexture(device, "back.png", &mTextureZN, &mTextureResourceViewZN, DXGI_FORMAT_R8G8B8A8_UNORM);
        D3DUtils::CreateTexture(device, "top.png", &mTextureYP, &mTextureResourceViewYP, DXGI_FORMAT_R8G8B8A8_UNORM);
        D3DUtils::CreateTexture(device, "bottom.png", &mTextureYN, &mTextureResourceViewYN, DXGI_FORMAT_R8G8B8A8_UNORM);
 
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
        for (int j = 0; j < thetaDivision; ++j) {
            verticies.push_back(top);
        }
        for (int i = 1; i < phiDivision; ++i) {
            float y = radius * cos(deltaPhi * i);
            float xzRadius = radius * sin(deltaPhi * i);
            SimpleVertex a;
            for (int j = 0; j < thetaDivision + 1; ++j) {
                a.Pos = { float(xzRadius * cos(j * deltaTheta)), y, float(xzRadius * sin(j * deltaTheta))};
                verticies.push_back(a);
            }
        }
        bottom.Pos = { 0.0f, -radius, 0.0f };
        for (int j = 0; j < thetaDivision; ++j) {
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
        bd.ByteWidth = sizeof(ViewConstantBuffer);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = device->CreateBuffer(&bd, NULL, &mViewContantBuffer);
        if (FAILED(hr)) {
            printf("CreateBuffer(ViewConstantBuffer) error : %08X\n", hr);
            throw std::runtime_error("");
        }
    }

    void Render(
        ID3D11DeviceContext* context,
        MathUtils::Matrix rotation,
        ID3D11Buffer* sharedContantBuffer
    )
    {
        ViewConstantBuffer wb;
        wb.Rotation = rotation.Transposed();
        context->UpdateSubresource(mViewContantBuffer, 0, NULL, &wb, 0, 0);

        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

        // Set index buffer
        context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

        // Set primitive topology
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->VSSetConstantBuffers(0, 1, &sharedContantBuffer);
        context->VSSetConstantBuffers(1, 1, &mViewContantBuffer);

        context->PSSetShaderResources(0, 1, &mTextureResourceViewXP);
        context->PSSetShaderResources(1, 1, &mTextureResourceViewXN);
        context->PSSetShaderResources(2, 1, &mTextureResourceViewZP);
        context->PSSetShaderResources(3, 1, &mTextureResourceViewZN);
        context->PSSetShaderResources(4, 1, &mTextureResourceViewYP);
        context->PSSetShaderResources(5, 1, &mTextureResourceViewYN);
 
        context->DrawIndexed(indiciesNum, 0, 0);
    }
};

