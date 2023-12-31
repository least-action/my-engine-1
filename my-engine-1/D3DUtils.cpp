#include <D3DCompiler.h>

#include <vector>
#include <stdexcept>
#include <stdio.h>
#include <d3d11.h>
#include <D3DCompiler.h>


namespace D3DUtils {
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

    void CreateVertexShaderWithInputLayout(
        ID3D11Device* device,
        LPCWSTR fileName,
        ID3D11VertexShader** vertexShader,
        const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
        ID3D11InputLayout** inputLayout
    )
    {
        LRESULT hr;

        ID3DBlob* pVSBlob = nullptr;
        // Compile the vertex shader
        hr = CompileShader(fileName, "main", "vs_4_0", &pVSBlob);
        if (FAILED(hr)) {
            printf("CompileShader error : %08X\n", hr);
            throw std::runtime_error("");
        }

        // Create the vertex shader
        hr = device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, vertexShader);
        if (FAILED(hr)) {
            pVSBlob->Release();
            printf("CreateVertexShader error : %08X\n", hr);
            throw std::runtime_error("");
        }

        // Create the input layout
        hr = device->CreateInputLayout(layout.data(), UINT(layout.size()), pVSBlob->GetBufferPointer(),
            pVSBlob->GetBufferSize(), inputLayout);
        pVSBlob->Release();
        if (FAILED(hr))
            throw std::runtime_error("");
    }

    void CreatePixelShader(
        ID3D11Device* device,
        LPCWSTR fileName,
        ID3D11PixelShader** pixelShader
    )
    {
        LRESULT hr;

        // Compile the pixel shader
        ID3DBlob* pPSBlob = nullptr;
        hr = D3DUtils::CompileShader(fileName, "main", "ps_4_0", &pPSBlob);
        if (FAILED(hr)) {
            printf("CompileShader error : %08X\n", hr);
            throw std::runtime_error("");
        }

        // Create the pixel shader
        hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, pixelShader);
        pPSBlob->Release();
        if (FAILED(hr)) {
            printf("CreatePixelShader error : %08X\n", hr);
            throw std::runtime_error("");
        }
    }
}
