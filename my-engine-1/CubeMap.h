#pragma once
#include <d3d11.h>
#include <stdexcept>
#include "MathUtils.h"
#include "D3DUtils.h"


const float PI2 = 3.14159265358979323846;

class CubeMap {
    float radius;
    struct SimpleVertex
    {
        MathUtils::Point Pos;
        MathUtils::TextCoord TextCoord;
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
    
    ID3D11Texture2D* mTexture = nullptr;
    ID3D11ShaderResourceView* mTextureResourceView = nullptr;

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
        D3DUtils::CreateTexture(device, "earth.png", &mTexture, &mTextureResourceView, DXGI_FORMAT_R8G8B8A8_UNORM);

        // build verticies and indices
        std::vector<SimpleVertex> verticies;
        int verticiesNum = 2 * thetaDivision + (thetaDivision + 1) * (phiDivision - 1);
        verticies.reserve(verticiesNum);
        std::vector<WORD> indicies;
        int indiciesNum = (thetaDivision * 2 * 3) + (thetaDivision * (phiDivision - 2) * 6);
        indicies.reserve(indiciesNum);
        this->indiciesNum = indiciesNum;

        float deltaTheta = 2.0f * PI2 / thetaDivision;
        float deltaPhi = PI2 / phiDivision;
        
        // verticies
        SimpleVertex top, bottom;
        top.Pos = {0.0f, radius, 0.0f};
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
                a.TextCoord = {1.0f / thetaDivision * j, 1.0f / phiDivision * i };
                verticies.push_back(a);
            }
        }
        bottom.Pos = { 0.0f, -radius, 0.0f };
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


        // Define the input layout
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        D3DUtils::CreateVertexShaderWithInputLayout(device, L"shaderCubeMapVertex.hlsl", &mVertexShader, layout, &mInputLayout);
        D3DUtils::CreatePixelShader(device, L"shaderCubeMapPixel.hlsl", &mPixelShader);

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
        ID3D11Buffer* sharedContantBuffer,
        ID3D11RasterizerState* rs,
        ID3D11RasterizerState* drs
    )
    {
        ViewConstantBuffer wb;
        wb.Rotation = rotation.Transposed();
        context->UpdateSubresource(mViewContantBuffer, 0, NULL, &wb, 0, 0);

        context->IASetInputLayout(mInputLayout);

        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

        // Set index buffer
        context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

        // Set primitive topology
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->RSSetState(rs);
        //context->RSSetState(drs);

        context->VSSetShader(mVertexShader, NULL, 0);
        context->VSSetConstantBuffers(0, 1, &sharedContantBuffer);
        context->VSSetConstantBuffers(1, 1, &mViewContantBuffer);
        context->PSSetShader(mPixelShader, NULL, 0);
        context->PSSetConstantBuffers(0, 1, &sharedContantBuffer);
        context->PSSetShaderResources(0, 1, &mTextureResourceView);
        context->DrawIndexed(indiciesNum, 0, 0);

        context->RSSetState(drs);
    }
};

class CubeMap2 {
	struct SimpleVertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 Normal;
        DirectX::XMFLOAT2 TexCoord;
	};

    struct ConstantBuffer
    {
        MathUtils::Matrix View;
        MathUtils::Matrix Projection;
    };

    ID3D11Buffer* mVertexBuffer = nullptr;
    ID3D11Buffer* mIndexBuffer = nullptr;
    ID3D11Buffer* mConstantBuffer = nullptr;
    ID3D11VertexShader* mVertexShader = nullptr;
    ID3D11PixelShader* mPixelShader = nullptr;
    ID3D11InputLayout* mInputLayout = nullptr;
    SimpleVertex vertices2[8] =
    {
        { {-1.0f, 1.0f, -1.0f} },
        { {1.0f, 1.0f, -1.0f} },
        { {1.0f, 1.0f, 1.0f} },
        { {-1.0f, 1.0f, 1.0f} },

        { {-1.0f, -1.0f, -1.0f} },
        { {1.0f, -1.0f, -1.0f} },
        { {1.0f, -1.0f, 1.0f} },
        { {-1.0f, -1.0f, 1.0f} }
    };
    SimpleVertex vertices[24] =
    {
        { {-2.0f, 2.0f, -2.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },
        { {2.0f, 2.0f, -2.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f} },
        { {2.0f, 2.0f, 2.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} },
        { {-2.0f, 2.0f, 2.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },

        { {-2.0f, -2.0f, -2.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },
        { {2.0f, -2.0f, -2.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} },
        { {2.0f, -2.0f, 2.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
        { {-2.0f, -2.0f, 2.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },

        { {-2.0f, -2.0f, 2.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
        { {-2.0f, -2.0f, -2.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
        { {-2.0f, 2.0f, -2.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
        { {-2.0f, 2.0f, 2.0f}, {-1.0f, 0.0f, 0.0f},{0.0f, 0.0f} },

        { {2.0f, -2.0f, 2.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
        { {2.0f, -2.0f, -2.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
        { {2.0f, 2.0f, -2.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
        { {2.0f, 2.0f, 2.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },

        { {-2.0f, -2.0f, -2.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },
        { {2.0f, -2.0f, -2.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },
        { {2.0f, 2.0f, -2.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
        { {-2.0f, 2.0f, -2.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },

        { {-2.0f, -2.0f, 2.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
        { {2.0f, -2.0f, 2.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },
        { {2.0f, 2.0f, 2.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },
        { {-2.0f, 2.0f, 2.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} },
    };

    WORD indices[36] =
    {
        3,1,0,
        2,1,3,

        6,4,5,
        7,4,6,

        11,9,8,
        10,9,11,

        14,12,13,
        15,12,14,

        19,17,16,
        18,17,19,

        22,20,21,
        23,20,22
    };

    ID3D11Texture2D* mTextureXP = nullptr;
    ID3D11Texture2D* mTextureXN = nullptr;
    ID3D11Texture2D* mTextureZP = nullptr;
    ID3D11Texture2D* mTextureZN = nullptr;
    ID3D11Texture2D* mTextureYP = nullptr;
    ID3D11Texture2D* mTextureYN = nullptr;
    ID3D11Texture2D* mTexture = nullptr;
    ID3D11ShaderResourceView* mTextureResourceView = nullptr;
    ID3D11ShaderResourceView* mTextureResourceViewXP = nullptr;
    ID3D11ShaderResourceView* mTextureResourceViewXN = nullptr;
    ID3D11ShaderResourceView* mTextureResourceViewZP = nullptr;
    ID3D11ShaderResourceView* mTextureResourceViewZN = nullptr;
    ID3D11ShaderResourceView* mTextureResourceViewYP = nullptr;
    ID3D11ShaderResourceView* mTextureResourceViewYN = nullptr;

public:
    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
    {
        D3DUtils::CreateTexture(device, "right.png", &mTextureXP, &mTextureResourceViewXP, DXGI_FORMAT_R8G8B8A8_UNORM);
        D3DUtils::CreateTexture(device, "left.png", &mTextureXN, &mTextureResourceViewXN, DXGI_FORMAT_R8G8B8A8_UNORM);
        D3DUtils::CreateTexture(device, "front.png", &mTextureZP, &mTextureResourceViewZP, DXGI_FORMAT_R8G8B8A8_UNORM);
        D3DUtils::CreateTexture(device, "back.png", &mTextureZN, &mTextureResourceViewZN, DXGI_FORMAT_R8G8B8A8_UNORM);
        D3DUtils::CreateTexture(device, "top.png", &mTextureYP, &mTextureResourceViewYP, DXGI_FORMAT_R8G8B8A8_UNORM);
        D3DUtils::CreateTexture(device, "bottom.png", &mTextureYN, &mTextureResourceViewYN, DXGI_FORMAT_R8G8B8A8_UNORM);
        
        // Define the input layout
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        D3DUtils::CreateVertexShaderWithInputLayout(device, L"shaderCubeMapVertex.hlsl", &mVertexShader, layout, &mInputLayout);
        D3DUtils::CreatePixelShader(device, L"shaderCubeMapPixel.hlsl", &mPixelShader);

        D3DUtils::CreateVertexBufferWithIndexBuffer(
            device,
            &mVertexBuffer, sizeof(SimpleVertex) * 24, vertices,
            &mIndexBuffer, sizeof(WORD) * 36, indices
        );

        // Create the constant buffer
        HRESULT hr;
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(ConstantBuffer);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = device->CreateBuffer(&bd, NULL, &mConstantBuffer);
        if (FAILED(hr)) {
            printf("CreateBuffer(mSurfaceWorldConstantBuffer) error : %08X\n", hr);
            throw std::runtime_error("");
        }
    }

    void Render(
        ID3D11DeviceContext* context,
        MathUtils::Matrix view, MathUtils::Matrix projection,
        ID3D11Buffer* sharedContantBuffer
    )
    {
        ConstantBuffer wb;
        wb.View = view.Transposed();
        wb.Projection = projection.Transposed();
        context->UpdateSubresource(mConstantBuffer, 0, NULL, &wb, 0, 0);

        context->IASetInputLayout(mInputLayout);
        
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
        
        // Set index buffer
        context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        
        // Set primitive topology
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->VSSetShader(mVertexShader, NULL, 0);
        //context->VSSetConstantBuffers(0, 1, &mConstantBuffer);
        context->VSSetConstantBuffers(0, 1, &sharedContantBuffer);
        context->PSSetShader(mPixelShader, NULL, 0);

        context->PSSetShaderResources(0, 1, &mTextureResourceViewXP);
        context->PSSetShaderResources(1, 1, &mTextureResourceViewXN);
        context->PSSetShaderResources(2, 1, &mTextureResourceViewZP);
        context->PSSetShaderResources(3, 1, &mTextureResourceViewZN);
        context->PSSetShaderResources(4, 1, &mTextureResourceViewYP);
        context->PSSetShaderResources(5, 1, &mTextureResourceViewYN);
        context->DrawIndexed(36, 0, 0);
    }
};
