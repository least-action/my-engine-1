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

        // Define the input layout
        std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        D3DUtils::CreateVertexShaderWithInputLayout(device, L"vertexShader.hlsl", &mVertexShader, layout, &mInputLayout);
        D3DUtils::CreatePixelShader(device, L"pixelShader.hlsl", &mPixelShader);

        D3DUtils::CreateVertexBufferWithIndexBuffer(
            device,
            &mVertexBuffer, sizeof(SimpleVertex) * 4, vertices,
            &mIndexBuffer, sizeof(WORD) * 6, indices
        );

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
