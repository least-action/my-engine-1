#include <directxmath.h>
#include <stdio.h>

namespace MathUtils {
	DirectX::XMVECTOR normalize3(const DirectX::XMFLOAT3& a)
	{
		return DirectX::XMVector4Normalize(XMLoadFloat3(&a));
	}

	DirectX::XMMATRIX MatrixLookAtLH(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 look, DirectX::XMFLOAT3 up)
	{
		DirectX::XMMATRIX i = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX translation = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-pos.x, -pos.y, -pos.z, 1.0f,
		};

		DirectX::XMVECTOR crossProduct = DirectX::XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&look));
		DirectX::XMFLOAT3 right;
		DirectX::XMStoreFloat3(&right, crossProduct);
		
		DirectX::XMMATRIX rotate = {
			right.x, up.x, look.x, 0.0f,
			right.y, up.y, look.y, 0.0f,
			right.z, up.z, look.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};
		
		return i * translation * rotate;
	}
}
