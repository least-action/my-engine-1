#pragma once
#include "D3DUtils.h"
#include "MathUtils.h"

#include <d3d11.h>
#include <directxmath.h>
#include <D3DCompiler.h>

#include <stdexcept>
#include <stdio.h>

const float PI = 3.14159265358979323846;

class Sphere {
    float radius;
    struct SimpleVertex
    {
        MathUtils::Point Pos;
        MathUtils::Point Normal;
    };

    struct WorldContantBuffer
    {
        MathUtils::Matrix mWorld;
    };

    struct Model
    {
        MathUtils::Point Pos;
    };

    ID3D11Buffer* mVertexBuffer = nullptr;
    ID3D11Buffer* mIndexBuffer = nullptr;
    ID3D11Buffer* worldContantBuffer = nullptr;
    ID3D11VertexShader* mVertexShader = nullptr;
    ID3D11PixelShader* mPixelShader = nullptr;
    ID3D11InputLayout* mInputLayout = nullptr;
    SimpleVertex vertices[24] =
    {
        { { -1.0f, 1.0f, -1.0f }, {0.0f, 1.0f, 0.0f}},
        { {1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },
        { {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} },
        { {-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} },

        { {-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f} },
        { {1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f} },
        { {1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
        { {-1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },

        { {-1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
        { {-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f} },
        { {-1.0f, 1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f} },
        { {-1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },

        { {1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} },
        { {1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f} },
        { {1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f} },
        { {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} },

        { {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f} },
        { {1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f} },
        { {1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f} },
        { {-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f} },

        { {-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} },
        { {1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} },
        { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} },
        { {-1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} },
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

    int thetaDivision, phiDivision;
    int indicesNum;

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
        // build verticies and indices
        std::vector<SimpleVertex> verticies2;
        int verticiesNum = thetaDivision * (phiDivision - 1) + 2;
        verticies2.reserve(verticiesNum);
        std::vector<WORD> indices2;
        int indiciesNum = (thetaDivision * 2 * 3) + (thetaDivision * (phiDivision - 2) * 6);
        indices2.reserve(indiciesNum);
        this->indicesNum = indiciesNum;

        float deltaTheta = 2.0f * PI / thetaDivision;
        float deltaPhi = PI / phiDivision;
        
        // verticies
        SimpleVertex top, bottom;
        top.Pos = {0.0f, radius, 0.0f};
        verticies2.push_back(top);
        for (int i = 1; i < phiDivision; ++i) {
            float y = radius * cos(deltaPhi * i);
            float xzRadius = radius * sin(deltaPhi * i);
            SimpleVertex a;
            for (int j = 0; j < thetaDivision; ++j) {
                a.Pos = { float(xzRadius * cos(j * deltaTheta)), y, float(xzRadius * sin(j * deltaTheta))};
                verticies2.push_back(a);
            }
        }
        bottom.Pos = { 0.0f, -radius, 0.0f };
        verticies2.push_back(bottom);

        // indices
        for (int j = 0; j < thetaDivision - 1; ++j) {
            indices2.push_back(0);
            indices2.push_back(j + 2);
            indices2.push_back(j + 1);
        }
        indices2.push_back(0);
        indices2.push_back(1);
        indices2.push_back(thetaDivision);

        for (int i = 0; i < phiDivision - 2; ++i) {
            int startOfUp = i * thetaDivision + 1;
            int startOfDown = startOfUp + thetaDivision;

            for (int j = 0; j < thetaDivision - 1; ++j) {
                indices2.push_back(startOfUp + j);
                indices2.push_back(startOfUp + j + 1);
                indices2.push_back(startOfDown + j);

                indices2.push_back(startOfUp + j + 1);
                indices2.push_back(startOfDown + j + 1);
                indices2.push_back(startOfDown + j);
            }

            indices2.push_back(startOfUp + thetaDivision - 1);
            indices2.push_back(startOfUp);
            indices2.push_back(startOfDown + thetaDivision - 1);

            indices2.push_back(startOfUp);
            indices2.push_back(startOfDown);
            indices2.push_back(startOfDown + thetaDivision - 1);
        }

        for (int j = 0; j < thetaDivision - 1; ++j) {
            indices2.push_back(verticiesNum - 1 - thetaDivision + j);
            indices2.push_back(verticiesNum - 1 - thetaDivision + 1 + j);
            indices2.push_back(verticiesNum - 1);
        }
        indices2.push_back(verticiesNum - 2);
        indices2.push_back(verticiesNum - 1 - thetaDivision);
        indices2.push_back(verticiesNum - 1);


        // Define the input layout
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        D3DUtils::CreateVertexShaderWithInputLayout(device, L"vertexShader.hlsl", &mVertexShader, layout, &mInputLayout);
        D3DUtils::CreatePixelShader(device, L"pixelShader.hlsl", &mPixelShader);

        D3DUtils::CreateVertexBufferWithIndexBuffer(
            device,
            &mVertexBuffer, sizeof(SimpleVertex) * verticiesNum, verticies2.data(),
            &mIndexBuffer, sizeof(WORD) * indiciesNum, indices2.data()
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

    void Render(ID3D11DeviceContext* context, ID3D11Buffer* sharedContantBuffer, ID3D11RasterizerState* rs, ID3D11RasterizerState* drs)
    {
        WorldContantBuffer wb;
        wb.mWorld = MathUtils::Matrix(
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                model.Pos.x, model.Pos.y, model.Pos.z, 1.0f,
            }
        ).Transposed();
        context->UpdateSubresource(worldContantBuffer, 0, NULL, &wb, 0, 0);

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
        context->VSSetConstantBuffers(1, 1, &worldContantBuffer);
        context->PSSetShader(mPixelShader, NULL, 0);
        context->PSSetConstantBuffers(0, 1, &sharedContantBuffer);
        context->DrawIndexed(indicesNum, 0, 0);

        context->RSSetState(drs);
    }
};
