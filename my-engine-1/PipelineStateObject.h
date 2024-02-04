#pragma once

#include <d3d11.h>

struct PipelineStateObject {
	ID3D11InputLayout* IL;
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3D11RasterizerState* RS;
};