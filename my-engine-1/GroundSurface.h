#pragma once

#include <d3d11.h>

class GroundSurface
{
    struct SimpleVertex
    {
        DirectX::XMFLOAT3 Pos;
        DirectX::XMFLOAT3 Normal;
    };

    struct WorldContantBuffer
    {
        DirectX::XMMATRIX mWorld;
    };

    ID3D11Buffer* mVertexBuffer = nullptr;
    ID3D11Buffer* mIndexBuffer = nullptr;
    ID3D11Buffer* worldContantBuffer = nullptr;
    ID3D11VertexShader* mVertexShader = nullptr;
    ID3D11PixelShader* mPixelShader = nullptr;
    ID3D11InputLayout* mInputLayout = nullptr;

    SimpleVertex vertices[8] =
    {
        { DirectX::XMFLOAT3(-50.0f, -1.0f, -50.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { DirectX::XMFLOAT3(50.0f, -1.0f, -50.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { DirectX::XMFLOAT3(50.0f, -1.0f, 50.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { DirectX::XMFLOAT3(-50.0f, -1.0f, 50.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { DirectX::XMFLOAT3(-50.0f, -1.0f, -50.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },
        { DirectX::XMFLOAT3(50.0f, -1.0f, -50.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },
        { DirectX::XMFLOAT3(50.0f, -1.0f, 50.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },
        { DirectX::XMFLOAT3(-50.0f, -1.0f, 50.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) }
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
        // Define the input layout
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        D3DUtils::CreateVertexShaderWithInputLayout(device, L"vertexShader.hlsl", &mVertexShader, layout, &mInputLayout);
        D3DUtils::CreatePixelShader(device, L"shaderGroundPixel.hlsl", &mPixelShader);

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
        wb.mWorld = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(0));
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
        context->DrawIndexed(12, 0, 0);
	}
};
