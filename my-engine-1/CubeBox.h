#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <D3DCompiler.h>

#include <stdexcept>
#include <stdio.h>



class CubeBox {
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

    HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
    {
        if (!srcFile || !entryPoint || !profile || !blob)
            return E_INVALIDARG;

        *blob = nullptr;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
        flags |= D3DCOMPILE_DEBUG;
#endif

        const D3D_SHADER_MACRO defines[] =
        {
            "EXAMPLE_DEFINE", "1",
            NULL, NULL
        };

        ID3DBlob* shaderBlob = nullptr;
        ID3DBlob* errorBlob = nullptr;
        HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entryPoint, profile,
            flags, 0, &shaderBlob, &errorBlob);
        if (FAILED(hr))
        {
            if (errorBlob)
            {
                OutputDebugStringA((char*)errorBlob->GetBufferPointer());
                errorBlob->Release();
            }

            if (shaderBlob)
                shaderBlob->Release();

            return hr;
        }

        *blob = shaderBlob;

        return hr;
    }

public:
    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
    {
        LRESULT hr;

        ID3DBlob* pVSBlob = nullptr;
        {
            // Compile the vertex shader
            hr = CompileShader(L"vertexShader.hlsl", "main", "vs_4_0", &pVSBlob);
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
            hr = CompileShader(L"pixelShader.hlsl", "main", "ps_4_0", &pPSBlob);
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
        

        SimpleVertex vertices[] =
        {
            { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
            { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },

            { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },
            { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },

            { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) },
            { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) },
            { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) },
            { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) },

            { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },

            { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },
            { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },

            { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
            { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
        };

        // Create vertex buffer
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SimpleVertex) * 24;
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
        WORD indices[] =
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

        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
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
        // Set index buffer
        context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

        // Set primitive topology
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->VSSetShader(mVertexShader, NULL, 0);
        context->VSSetConstantBuffers(0, 1, &constantBuffer);
        context->PSSetShader(mPixelShader, NULL, 0);
        context->PSSetConstantBuffers(0, 1, &constantBuffer);
        context->DrawIndexed(36, 0, 0);
    }
};
