#pragma once

#include <directxmath.h>

namespace MathUtils {
	DirectX::XMMATRIX MatrixLookAtLH(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 look, DirectX::XMFLOAT3 up);
}
