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

    void CreatePixelShader(
        ID3D11Device* device,
        LPCWSTR fileName,
        ID3D11PixelShader** pixelShader
    );
}
