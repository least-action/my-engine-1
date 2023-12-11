#pragma once

#include <D3DCompiler.h>

namespace D3DUtils {
    HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
}
