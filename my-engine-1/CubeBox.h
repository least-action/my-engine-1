#pragma once
#include "D3DUtils.h"

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
        DirectX::XMFLOAT2 TexCoord;
	};

    struct WorldContantBuffer
    {
        DirectX::XMMATRIX mWorld;
    };
    
    struct Model
    {
        DirectX::XMFLOAT3 Pos;
    };

    ID3D11Buffer* mVertexBuffer = nullptr;
    ID3D11Buffer* mIndexBuffer = nullptr;
    ID3D11Buffer* worldContantBuffer = nullptr;
    ID3D11VertexShader* mVertexShader = nullptr;
    ID3D11PixelShader* mPixelShader = nullptr;
    ID3D11InputLayout* mInputLayout = nullptr;
    SimpleVertex vertices[24] =
    {
        { {-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },
        { {1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f} },
        { {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} },
        { {-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },

        { {-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },
        { {1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} },
        { {1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
        { {-1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },

        { {-1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
        { {-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
        { {-1.0f, 1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
        { {-1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f},{0.0f, 0.0f} },

        { {1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
        { {1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
        { {1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
        { {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },

        { {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },
        { {1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },
        { {1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
        { {-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },

        { {-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
        { {1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },
        { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },
        { {-1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} },
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

    ID3D11Texture2D* mTexture = nullptr;
    ID3D11ShaderResourceView* mTextureResourceView = nullptr;
    ID3D11SamplerState* mSamplerState = nullptr;

public:
    Model model = Model();

    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
    {
        D3DUtils::CreateTexture(device, "woodbox.png", &mTexture, &mTextureResourceView, DXGI_FORMAT_R8G8B8A8_UNORM);
        
        // Define the input layout
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        D3DUtils::CreateVertexShaderWithInputLayout(device, L"shaderContainerVertex.hlsl", &mVertexShader, layout, &mInputLayout);
        D3DUtils::CreatePixelShader(device, L"shaderContainerPixel.hlsl", &mPixelShader);

        D3DUtils::CreateVertexBufferWithIndexBuffer(
            device,
            &mVertexBuffer, sizeof(SimpleVertex) * 32, vertices,
            &mIndexBuffer, sizeof(WORD) * 36, indices
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
        wb.mWorld = DirectX::XMMatrixTranspose(
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                model.Pos.x, model.Pos.y, model.Pos.z, 1.0f,
            }
        );
        context->UpdateSubresource(worldContantBuffer, 0, NULL, &wb, 0, 0);

        context->IASetInputLayout(mInputLayout);
        
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
        
        // Set index buffer
        context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        
        // Set primitive topology
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->VSSetShader(mVertexShader, NULL, 0);
        context->VSSetConstantBuffers(0, 1, &sharedContantBuffer);
        context->VSSetConstantBuffers(1, 1, &worldContantBuffer);
        context->PSSetShader(mPixelShader, NULL, 0);
        context->PSSetConstantBuffers(0, 1, &sharedContantBuffer);

        context->PSSetShaderResources(0, 1, &mTextureResourceView);
        context->DrawIndexed(36, 0, 0);
    }
};
