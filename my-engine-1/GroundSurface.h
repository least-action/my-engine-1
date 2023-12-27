#pragma once

#include <d3d11.h>

class GroundSurface
{
    struct SimpleVertex
    {
        DirectX::XMFLOAT3 Pos;
        DirectX::XMFLOAT3 Normal;
    };

    ID3D11Buffer* mVertexBuffer = nullptr;
    ID3D11Buffer* mIndexBuffer = nullptr;
    ID3D11VertexShader* mVertexShader = nullptr;
    ID3D11PixelShader* mPixelShader = nullptr;
    ID3D11InputLayout* mInputLayout = nullptr;

    SimpleVertex vertices[4] =
    {
        { DirectX::XMFLOAT3(-10.0f, -1.0f, -10.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { DirectX::XMFLOAT3(10.0f, -1.0f, -10.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { DirectX::XMFLOAT3(10.0f, -1.0f, 10.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { DirectX::XMFLOAT3(-10.0f, -1.0f, 10.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
    };

    WORD indices[6] =
    {
        3,1,0,
        2,1,3,
    };

public:
    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
    {
        LRESULT hr;

        ID3DBlob* pVSBlob = nullptr;
        {
            // Compile the vertex shader
            hr = D3DUtils::CompileShader(L"vertexShader.hlsl", "main", "vs_4_0", &pVSBlob);
            if (FAILED(hr)) {
                printf("CompileShader error : %08X\n", hr);
                throw std::runtime_error("");
            }

            // Create the vertex shader
            hr = device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &mVertexShader);
            if (FAILED(hr))
            {
                pVSBlob->Release();
                printf("CreateVertexShader error : %08X\n", hr);
                throw std::runtime_error("");
            }
        }

        {
            // Define the input layout
            D3D11_INPUT_ELEMENT_DESC layout[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };
            UINT numElements = ARRAYSIZE(layout);

            // Create the input layout
            hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                pVSBlob->GetBufferSize(), &mInputLayout);
            pVSBlob->Release();
            if (FAILED(hr))
                throw std::runtime_error("");

            // Set the input layout
            context->IASetInputLayout(mInputLayout);
        }

        {
            // Compile the pixel shader
            ID3DBlob* pPSBlob = nullptr;
            hr = D3DUtils::CompileShader(L"pixelShader.hlsl", "main", "ps_4_0", &pPSBlob);
            if (FAILED(hr)) {
                printf("CompileShader error : %08X\n", hr);
                throw std::runtime_error("");
            }

            // Create the pixel shader
            hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &mPixelShader);
            pPSBlob->Release();
            if (FAILED(hr)) {
                printf("CreatePixelShader error : %08X\n", hr);
                throw std::runtime_error("");
            }
        }

        // Create vertex buffer
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SimpleVertex) * 4;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = vertices;
        hr = device->CreateBuffer(&bd, &InitData, &mVertexBuffer);
        if (FAILED(hr)) {
            printf("CreateBuffer(vertex buffer) error : %08X\n", hr);
            throw std::runtime_error("");
        }

        // Set vertex buffer
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

        // Create index buffer
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * 6;        // 36 vertices needed for 12 triangles in a triangle list
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        InitData.pSysMem = indices;
        hr = device->CreateBuffer(&bd, &InitData, &mIndexBuffer);
        if (FAILED(hr)) {
            printf("CreateBuffer(index buffer) error : %08X\n", hr);
            throw std::runtime_error("");
        }

        return;
    }

	void Render(ID3D11DeviceContext* context, ID3D11Buffer* constantBuffer)
	{
        
        context->IASetInputLayout(mInputLayout);
        
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
        
        // Set index buffer
        context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

        // Set primitive topology
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
       

        context->VSSetShader(mVertexShader, NULL, 0);
        context->VSSetConstantBuffers(0, 1, &constantBuffer);
        context->PSSetShader(mPixelShader, NULL, 0);
        context->PSSetConstantBuffers(0, 1, &constantBuffer);
        context->DrawIndexed(6, 0, 0);
	}
};
