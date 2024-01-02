#pragma once

#include <directxmath.h>

namespace MathUtils {
	DirectX::XMMATRIX MatrixLookAtLH(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 at, DirectX::XMFLOAT4 up);
}
