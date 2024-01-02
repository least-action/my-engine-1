#include <directxmath.h>

namespace MathUtils {
	DirectX::XMMATRIX MatrixLookAtLH(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 at, DirectX::XMFLOAT4 up)
	{
		return {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 10.0f, 1.0f,
		};
	}
}
