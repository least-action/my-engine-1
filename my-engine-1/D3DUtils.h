#pragma once

#include <vector>
#include <d3d11.h>


namespace D3DUtils {
    HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
    
    void CreateVertexShaderWithInputLayout(
        ID3D11Device* device,
        LPCWSTR fileName,
        ID3D11VertexShader** vertexShader,
        const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
        ID3D11InputLayout** inputLayout
    );

    void CreateGeometryShader(
        ID3D11Device* device,
        LPCWSTR fileName,
        ID3D11GeometryShader** shader
    );

    void CreatePixelShader(
        ID3D11Device* device,
        LPCWSTR fileName,
        ID3D11PixelShader** pixelShader
    );

    void CreateVertexBufferWithIndexBuffer(
        ID3D11Device* device,
        ID3D11Buffer** vertexBuffer,
        UINT vertexBufferByteWidth,
        const void* vertexDataMem,
        ID3D11Buffer** indexBuffer,
        UINT indexBufferByteWidth,
        const void* indexDataMem
    );

    void CreateTexture(
        ID3D11Device* device,
        const char* fileName,
        ID3D11Texture2D** texture,
        ID3D11ShaderResourceView** resourceView,
        DXGI_FORMAT format
    );

    void CreateDepthOnlyTexture(
        UINT width,
        UINT height,
        ID3D11Device* device,
        ID3D11Texture2D** texture,
        ID3D11ShaderResourceView** resourceView,
        DXGI_FORMAT format
    );
}
