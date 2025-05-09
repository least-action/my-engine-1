#include <vector>
#include <stdexcept>
#include <cstring>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

#include <D3DCompiler.h>
#include <stdio.h>
#include <d3d11.h>
#include <D3DCompiler.h>

#include <wincodec.h>
#include <wincodecsdk.h>
#include <combaseapi.h>
#include <winerror.h>


namespace D3DUtils {
    HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
    {
        if (!srcFile || !entryPoint || !profile || !blob)
            return E_INVALIDARG;

        *blob = nullptr;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
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
        if (FAILED(hr)) {
            std::wcout << fileName << std::endl;
            printf("CreateInputLayout error : %08X\n", hr);
            throw std::runtime_error("");
        }
    }

    void CreateGeometryShader(
        ID3D11Device* device,
        LPCWSTR fileName,
        ID3D11GeometryShader** shader
    )
    {
        LRESULT hr;

        // Compile the geometry shader
        ID3DBlob* pGSBlob = nullptr;
        hr = D3DUtils::CompileShader(fileName, "main", "gs_4_0", &pGSBlob);
        if (FAILED(hr)) {
            printf("CompileShader error : %08X\n", hr);
            throw std::runtime_error("");
        }

        // Create the geometry shader
        hr = device->CreateGeometryShader(pGSBlob->GetBufferPointer(), pGSBlob->GetBufferSize(), nullptr, shader);
        pGSBlob->Release();
        if (FAILED(hr)) {
            printf("CreateGeometryShader error : %08X\n", hr);
            throw std::runtime_error("");
        }
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
        hr = D3DUtils::CompileShader(fileName, "main", "ps_4_0_level_9_3", &pPSBlob);
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

    void CreateVertexBufferWithIndexBuffer(
        ID3D11Device* device,
        ID3D11Buffer** vertexBuffer,
        UINT vertexBufferByteWidth,
        const void* vertexDataMem,
        ID3D11Buffer** indexBuffer,
        UINT indexBufferByteWidth,
        const void* indexDataMem
    )
    {
        LRESULT hr;
        
        // create vertex buffer
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = vertexBufferByteWidth;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = vertexDataMem;
        hr = device->CreateBuffer(&bd, &InitData, vertexBuffer);
        if (FAILED(hr)) {
            printf("CreateBuffer(vertex buffer) error : %08X\n", hr);
            throw std::runtime_error("");
        }

        // Create index buffer
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = indexBufferByteWidth;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        InitData.pSysMem = indexDataMem;
        hr = device->CreateBuffer(&bd, &InitData, indexBuffer);
        if (FAILED(hr)) {
            printf("CreateBuffer(index buffer) error : %08X\n", hr);
            throw std::runtime_error("");
        }
    }

    void CreateTexture(
        ID3D11Device* device,
        const char* fileName,
        ID3D11Texture2D** texture,
        ID3D11ShaderResourceView** resourceView,
        DXGI_FORMAT format
    )
    {
        LRESULT hr;

        int width, height, comp;
        unsigned char* img = stbi_load(fileName, &width, &height, &comp, 0);

        std::vector<uint8_t> image;
        image.resize(width * height * comp);
        memcpy(image.data(), img, image.size() * sizeof(uint8_t));

        // Create texture
        /*
        * todo: check iamge format
        * https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-textures-how-to
        */

        D3D11_TEXTURE2D_DESC desc;
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = image.data();
        initData.SysMemPitch = desc.Width * sizeof(uint8_t) * comp;
        //initData.SysMemSlicePitch = static_cast<UINT>(imageSize);

        hr = device->CreateTexture2D(&desc, &initData, texture);
        if (FAILED(hr) || texture == 0) {
            printf("CreateTexture2D error : %08X\n", hr);
            throw std::runtime_error("");
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
        memset(&SRVDesc, 0, sizeof(SRVDesc));
        SRVDesc.Format = format;
        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        SRVDesc.Texture2D.MipLevels = 1;

        hr = device->CreateShaderResourceView(*texture, &SRVDesc, resourceView);
        if (FAILED(hr))
        {
            (*texture)->Release();
            printf("CreateShaderResourceView error : %08X\n", hr);
            throw std::runtime_error("");
        }
    }

    void CreateDepthOnlyTexture(
        UINT width,
        UINT height,
        ID3D11Device* device,
        ID3D11Texture2D** texture,
        ID3D11ShaderResourceView** resourceView,
        DXGI_FORMAT format
    )
    {
    }
}
