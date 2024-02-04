#pragma once

#include <d3d11.h>

class GroundSurface
{
    struct SimpleVertex
    {
        MathUtils::Point Pos;
        MathUtils::Vector Normal;
        MathUtils::TextCoord TextCoord;
    };

    struct WorldContantBuffer
    {
        DirectX::XMMATRIX World;
    };

    ID3D11Buffer* mVertexBuffer = nullptr;
    ID3D11Buffer* mIndexBuffer = nullptr;
    ID3D11Buffer* worldContantBuffer = nullptr;
    ID3D11VertexShader* mVertexShader = nullptr;
    ID3D11PixelShader* mPixelShader = nullptr;
    ID3D11InputLayout* mInputLayout = nullptr;

    SimpleVertex vertices[8] =
    {
        { {MathUtils::SCALE * -2.5f,MathUtils::SCALE * -0.05f, MathUtils::SCALE * -2.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },
        { {MathUtils::SCALE * 2.5f, MathUtils::SCALE * -0.05f, MathUtils::SCALE * -2.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f} },
        { {MathUtils::SCALE * 2.5f, MathUtils::SCALE * -0.05f, MathUtils::SCALE * 2.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} },
        { {MathUtils::SCALE * -2.5f,MathUtils::SCALE * -0.05f, MathUtils::SCALE * 2.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
        { {MathUtils::SCALE * -2.5f,MathUtils::SCALE * -0.05f, MathUtils::SCALE * -2.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },
        { {MathUtils::SCALE * 2.5f, MathUtils::SCALE * -0.05f, MathUtils::SCALE * -2.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} },
        { {MathUtils::SCALE * 2.5f, MathUtils::SCALE * -0.05f, MathUtils::SCALE * 2.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
        { {MathUtils::SCALE * -2.5f,MathUtils::SCALE * -0.05f, MathUtils::SCALE * 2.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} }
    };

    WORD indices[12] =
    {
        3,1,0,
        2,1,3,
        4,5,7,
        7,5,6
    };

public:
    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
    {
        D3DUtils::CreateVertexBufferWithIndexBuffer(
            device,
            &mVertexBuffer, sizeof(SimpleVertex) * 8, vertices,
            &mIndexBuffer, sizeof(WORD) * 12, indices
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
        wb.World = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(0));
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
        context->DrawIndexed(12, 0, 0);
	}
};
