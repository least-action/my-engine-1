#pragma once

#include <directxmath.h>

namespace MathUtils {
	DirectX::XMMATRIX MatrixLookAtLH(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 look, DirectX::XMFLOAT3 up, DirectX::XMFLOAT3 right);
	DirectX::XMMATRIX BuildRotation(DirectX::XMFLOAT3 axis, float theta);
	void PrintMatrix(DirectX::XMMATRIX m);
	void PrintFloat3(DirectX::XMFLOAT3 f);
}
